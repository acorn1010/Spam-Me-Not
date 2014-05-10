#ifndef __SPAM_FILTER_H
#define __SPAM_FILTER_H

#include <list>
#include <set>
#include <string>

class SpamFilter {
public:
    bool isSpam(const std::string &message);
    SpamFilter();

protected:
    std::multiset<std::string> spamSet;
    std::set<std::string> spamUnique;
    /** The number of words that appear in each spam message */
    std::list<int> spamWordCount;
    /** The average word count of a normal person.  TODO: Get the real value */
    float spamWordAverage;
    float normalWordAverage;
    std::list<std::string> getWordsFromMessage(const std::string &message);

    bool isEmptyWord(const std::string &word);

    /**
     * Calculates spam probability by counting the number of times each word
     * appears in the known spam set.  Returns a result from 0 to 1 inclusive,
     * where a higher value is more indicative of spam.
     */
    float SpamFilter::getSpamProbability(const std::string &message);

private:
    void init();
};

#endif // __SPAM_FILTER_H