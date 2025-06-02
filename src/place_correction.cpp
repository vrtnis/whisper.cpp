#include "place_correction.h"
#include "double_metaphone.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <sstream>

using namespace std;

static unordered_multimap<string,string> g_index;

static string to_lower(const string &s) {
    string r=s; for(char &c:r) c=tolower((unsigned char)c); return r; }

static string number_to_words(int n) {
    static const char* words[] = {"zero","one","two","three","four","five","six","seven","eight","nine","ten","eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen","eighteen","nineteen","twenty"};
    if (n>=0 && n<=20) return words[n];
    return std::to_string(n);
}

vector<string> normalize_numbers(const string &text) {
    vector<string> out; string token; size_t i=0;
    while(i<text.size()) {
        if (isalnum((unsigned char)text[i])) {
            size_t start=i; while(i<text.size() && isalnum((unsigned char)text[i])) i++; token=text.substr(start,i-start);
            bool isnum = !token.empty() && all_of(token.begin(), token.end(), ::isdigit);
            if(isnum) out.push_back(number_to_words(stoi(token))); else out.push_back(token);
        } else { i++; }
    }
    return out;
}

void build_phonetic_index(const vector<string> &glossary) {
    g_index.clear();
    for(const auto &w: glossary){
        auto codes = doublemetaphone::double_metaphone(to_lower(w));
        g_index.emplace(codes.first, w);
    }
}

static size_t levenshtein(const string &s1, const string &s2) {
    const size_t m=s1.size(), n=s2.size();
    vector<size_t> dp(n+1);
    for(size_t j=0;j<=n;++j) dp[j]=j;
    for(size_t i=1;i<=m;++i){
        size_t prev=dp[0]; dp[0]=i; size_t temp;
        for(size_t j=1;j<=n;++j){
            temp=dp[j];
            if(s1[i-1]==s2[j-1]) dp[j]=prev; else dp[j]=min({dp[j-1],dp[j],prev})+1;
            prev=temp;
        }
    }
    return dp[n];
}

struct Token { string text; size_t start; size_t end; };

static vector<Token> tokenize(const string &s){
    vector<Token> tokens; size_t i=0; while(i<s.size()){
        if(isalnum((unsigned char)s[i])){ size_t start=i; while(i<s.size() && isalnum((unsigned char)s[i])) i++; tokens.push_back({s.substr(start,i-start),start,i}); } else { i++; }
    } return tokens; }

static string join_tokens(const vector<Token>& t, size_t i, size_t len){
    string r; for(size_t k=0;k<len;k++){ if(k>0) r+=' '; r+=t[i+k].text; } return r; }

string correct_place_names(const string &segment) {
    if (g_index.empty()) return segment;
    string result = segment;

    bool changed = true;
    while (changed) {
        changed = false;
        auto tokens = tokenize(result);
        for (size_t i = 0; i < tokens.size(); ++i) {
            for (int len = 3; len >= 1 && i + len <= tokens.size(); --len) {
                string phrase = join_tokens(tokens, i, len);
                string code = doublemetaphone::double_metaphone(to_lower(phrase)).first;
                auto range = g_index.equal_range(code);
                string best; size_t bestDist = SIZE_MAX;
                for (auto it = range.first; it != range.second; ++it) {
                    string cand = it->second;
                    size_t dist = levenshtein(to_lower(phrase), to_lower(cand));
                    if (dist < bestDist) { bestDist = dist; best = cand; }
                }
                if (!best.empty() && bestDist <= 2 && to_lower(phrase) != to_lower(best)) {
                    size_t start = tokens[i].start;
                    size_t end = tokens[i + len - 1].end;
                    result.replace(start, end - start, best);
                    changed = true;
                    break;
                }
            }
            if (changed) break;
        }
    }

    return result;
}
