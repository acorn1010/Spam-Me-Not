#include "SpamFilter.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace {
    void logMessage(const std::string &message) {
        std::ofstream outfile;
        outfile.open("spamlog.txt", std::ios_base::app);
        outfile << message << std::endl;
    }

    void logMessage(const float probability, const std::string &message) {
        std::ofstream outfile;
        outfile.open("spamlog.txt", std::ios_base::app);
        outfile << probability << ":  " << message << std::endl;
        // File closes itself automatically at destruction
    }
}

bool SpamFilter::isEmptyWord(const std::string &word) {
    bool result = true;

    for (std::size_t i = 0; i < word.size() && result; ++i) {
        result = word[i] == ' ' || word[i] == '\t';
    }

    return result;
}

std::list<std::string> SpamFilter::getWordsFromMessage(const std::string &message) {
    std::list<std::string> result;

    std::string word = "";
    // Break up the spam messages into regions of a-zA-Z0-9 and symbols.
    bool oldAlphaNumeric = true;
    for (char c : message) {
        bool alphaNumeric = isalnum(c) != 0;

        // If the same type
        if (alphaNumeric ^ !oldAlphaNumeric) {
            // Limit length of non-alphanumeric
            if (!alphaNumeric && word.size() > 0) {
                result.push_back(word);
                word = "";
            }
            if (c != ' ' || c != '\t') {
                word += c;
            }
        } else { // Different type!
            if (!isEmptyWord(word)) {
                result.push_back(word);
            }
            if (c == ' ' || c == '\t') {
                word = "";
            } else {
                word = c;
            }
        }
        oldAlphaNumeric = alphaNumeric;
    }

    // Don't forget the last word
    if (!isEmptyWord(word)) {
        result.push_back(word);
    }

    return result;
}

/**
 * Calculates spam probability by counting the number of times each word
 * appears in the known spam set.  Returns a result around 0 to 1, where
 * a higher value is more indicative of spam.  Results are not bound to 0 or 1.
 */
float SpamFilter::getSpamProbability(const std::string &message) {
    float result = 0.0;
    std::list<std::string> words = getWordsFromMessage(message);
    float multiplier = 1.0f;
    float spamWords = 0;

    // Guard.  Empty messages aren't spam
    if (words.empty()) {
        return 0;
    }

    // Ideally we'd use statistics and a normalized curve, but then again
    // ideally we'd be using SGD Classifier / AdaBoost or something...  I'll 
    // need a lot of test points for this (should be easy enough with chat logs).
    if (normalWordAverage > words.size()) {
        multiplier = 1 + (1.0f / normalWordAverage) * (words.size() - normalWordAverage);
    }

    for (auto word : words) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        int count = spamSet.count(word);

        // It sure would be nice to have the probabilities for non-spam messages...
        if (count > 0) {
            //float wordProbability = 1.0f * count / (spamSet.size() / spamUnique.size());
            //spamWords += wordProbability;
            //logMessage("WORD: " + word + " (" + std::to_string(wordProbability) + ")");
            ++spamWords;
        }
    }

    return 1.0f * spamWords / words.size() * multiplier;
}

bool SpamFilter::isSpam(const std::string &message) {
    float probability = getSpamProbability(message);
    logMessage(probability, message);
    return probability > 0.6f;
}

SpamFilter::SpamFilter() {
    init();
}

void SpamFilter::init() {
    std::list<std::string> spamMessages;
    spamMessages.push_back("Hi, WWW,GKplayer,{0M, 1000K Gil + 50K Bonus Gil just 7.9 USD,delivery time:10 Minutes - shipping via in-game mailbox. WWW,GKplayer,{0M");
    spamMessages.push_back("FFXIV Gil 1M=9$~(Wind,Water,Lightning,Ice,Fire,Earth;Shard*500&Crystal*100)=1$~~(Powerleveling 1-50==159$~~~LFDPS,COM~~~ ||rn6onokm");
    spamMessages.push_back("wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  wvvw.ffl4goldma｜｜.c0m  sell cheap gil ,Delivery in 10 minutes.EaN");
    spamMessages.push_back("WWW.T I-I E P 0 W E R L E V E L.C0M  1000K Gil just 8.9 USD,Any purchase over 20 USD gets 12% bonus gil. delivery time:10 Minutes - shipping via in-game mailbox.");
    spamMessages.push_back("【ＷＷＷ.ＰＶＰＢＡИＫ.ＣＯＭ】【５０００K　８％ＯＦＦ≈３５-５０Ｕ‘sd】【２０％ ＯＦＦ！ １ Ｄａｙ Ｏｎｌｙ】 【Cheapest FF14 Gil and PowerLevelíng】【Real 5mins Delivery Guaranteed】【100% Handworl< Guaranteed】 Good Luck! CC1A");
    spamMessages.push_back("WWW. E U S G A /\\/\\ E .COM Selling the lowest priced Gil.Any purchase over $40 gets 14% bonus gil, Use discount code:d8ad335f4ace  (repeatable before Jun 1st,2014) and enjoy 10% discount and Guranteed instant delivery-");

    normalWordAverage = 20.0f;
    spamSet.clear();
    spamUnique.clear();
    std::string word = "";
    // Break up the spam messages into regions of a-zA-Z0-9 and symbols.
    for (auto message : spamMessages) {
        std::list<std::string> words = getWordsFromMessage(message);
        for (auto word : words) {
            // Convert to lowercase before insertion into the spam set
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            spamSet.insert(word);
            spamUnique.insert(word);
        }
        spamWordCount.push_back(words.size());
    }

    spamWordAverage = 0;
    for (auto wordCount : spamWordCount) {
        spamWordAverage += wordCount;
    }
    if (spamWordCount.size() > 0) {
        spamWordAverage /= spamWordCount.size();
    }
}
