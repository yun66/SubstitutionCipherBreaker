#include <iostream>
#include <cctype>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <fstream>
#include <vector>
#include <sstream>

std::unordered_map<std::string, std::list<std::string>> dictionary;
std::string normalize(std::string s)
{
    auto C = 'A';
    auto i = s.begin();

    for (auto c : s)
    {
        if (std::isupper(c))
        {
            continue;
        }

        std::replace(i++, s.end(), c, C++);
    }

    return s;
}
std::list<std::string> get_candidates(std::string &s)
{
    return std::list<std::string>(dictionary[normalize(s)]);
}

struct cstring
{
    cstring(std::string w)
    {
        word = w;
        candidates = dictionary[normalize(word)];

        for (unsigned i = 0; i < word.length(); i++)
        {
            for (auto j : candidates)
            {
                letter_map[word[i]].emplace(j[i]);
            }
        }
    }

    std::string word;
    std::list<std::string> candidates;
    std::unordered_map<char, std::set<char>> letter_map;
};

std::set<char> intersect(std::set<char> &a, std::set<char> &b)
{
    std::set<char> c;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    return c;
}
std::string apply_map(std::string &cipher, std::string &plain, std::unordered_map<char, char> &map)
{
    std::string result = "";

    for (unsigned i = 0; i < cipher.length(); i++)
    {
        result += map.find(cipher[i]) == map.end() ? plain[i] : map[cipher[i]];
    }

    return result;
}
std::unordered_map<char, char> update_map(std::string &cipher, std::string &plain, std::unordered_map<char, char> map)
{
    for (unsigned i = 0; i < cipher.length(); i++)
    {
        if (map.find(cipher[i]) == map.end())
        {
            map[cipher[i]] = plain[i];
        }
    }

    return map;
}
bool is_consistent(std::unordered_map<char, char> &map)
{
    std::unordered_map<char, char> counter;

    for (auto &i : map)
    {
        counter[i.second] = i.first;
    }

    return map.size() == counter.size();
}
void unscramble(std::vector<cstring> &cipher_words, unsigned depth, std::unordered_map<char, char> map)
{
    if (!is_consistent(map))
    {
        return;
    }

    if (depth >= cipher_words.size())
    {
        for (auto &i : cipher_words)
        {
            for (auto &j : i.word)
            {
                std::cout << map[j];
            }

            std::cout << ' ';
        }

        std::cout << std::endl;
    }
    else
    {
        for (auto &i : cipher_words[depth].candidates)
        {
            if (apply_map(cipher_words[depth].word, i, map) == i)
            {
                unscramble(cipher_words, depth + 1, update_map(cipher_words[depth].word, i, map));
            }
        }
    }
}

int main()
{
    std::cout << "Initializing dictionary..." << std::endl;

    std::ifstream file("EnglishDictionary.txt");
    std::string word;

    file >> word;

    while (file)
    {
        dictionary[normalize(word)].push_back(word);
        file >> word;
    }

    file.close();

    std::cout << "Enter ciphertext:" << std::endl << "> ";

    std::string sentence;

    std::getline(std::cin, sentence);
    std::cout << "Preparing..." << std::endl;

    std::istringstream iss(sentence);
    std::vector<cstring> cipher_words
    {
        std::istream_iterator < std::string > { iss },
        std::istream_iterator < std::string > { }
    };

    std::cout << "Pruning..." << std::endl;

    std::unordered_map<char, std::set<char>> pruner;

    for (char i = 'a'; i <= 'z'; i++)
    {
        for (char j = 'a'; j <= 'z'; j++)
        {
            pruner[i].insert(j);
        }
    }

    for (auto &i : cipher_words)
    {
        for (auto &j : i.word)
        {
            pruner[j] = intersect(pruner[j], i.letter_map[j]);
        }
    }

    for (auto &i : cipher_words)
    {
        for (unsigned j = 0; j < i.word.length(); j++)
        {
            for (auto k = i.candidates.begin(); k != i.candidates.end();)
            {
                if (pruner[i.word[j]].find((*k)[j]) == pruner[i.word[j]].end())
                {
                    k = i.candidates.erase(k);
                }
                else
                {
                    k++;
                }
            }
        }
    }

    std::cout << "Breaking..." << std::endl << std::endl;

    unscramble(cipher_words, 0, std::unordered_map<char, char>());

    std::cout << std::endl << "The program has finished.";

    std::cin.get();

    return 0;
}