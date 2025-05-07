
#include <gtest/gtest.h>
#include "../FileManager/BlacklistFileManager.h"
#include "../FileManager/BloomFilterFileManager.h"
#include "../BloomFilter/BloomFilter.h"
#include "../HashFunctions/HashFunctionFactory.h"
#include "../UrlCheck/url_blacklist.h"
#include "../UrlCheck/url_validation.h"
#include "../server/server_arg_validation.h"
#include "../requests/requestCheck.h"
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// Simulate command by reusing actual server logic
std::string simulateCommand(const std::string& input,
                            BlacklistURL& blacklist,
                            BlacklistFileManager& manager,
                            BloomFilterFileManager& bloomManager) {
    return requestCheck(input, blacklist, manager, bloomManager);
}


TEST(ServerCommandTests, TestPostAndGet) {
    auto hashFuncs = std::vector<std::shared_ptr<HashFunction>>{
        MakeHash::createHashFunction(1),
        MakeHash::createHashFunction(2)
    };
    auto bloom = std::make_shared<BloomFilter>(100, hashFuncs);
    BlacklistURL blacklist(bloom);
    BlacklistFileManager blManager("../tmp/test_blacklist.txt");
    BloomFilterFileManager bloomManager("../tmp/test_bloom.txt");

    // check if adding an unvalid url, expect return false
    EXPECT_NE(simulateCommand("POST www.", blacklist, blManager, bloomManager), "201 Created\n");


    // check if the url exist, should return false because the list is empty
    EXPECT_NE(simulateCommand("GET www.example.com0", blacklist, blManager, bloomManager), "200 Ok\n\ntrue true\n");

    // adding new url, expect to return true
    EXPECT_EQ(simulateCommand("POST google.com", blacklist, blManager, bloomManager), "201 Created\n");
    
    // adding an exist url.
    EXPECT_EQ(simulateCommand("POST google.com", blacklist, blManager, bloomManager), "200 Ok\n");


    // check the url if exist, expect to return true because we already add it
    EXPECT_EQ(simulateCommand("GET google.com", blacklist, blManager, bloomManager), "200 Ok\n\ntrue true\n");
}

TEST(ServerCommandTests, TestBadRequest) {
    auto hashFuncs = std::vector<std::shared_ptr<HashFunction>>{
        MakeHash::createHashFunction(1),
        MakeHash::createHashFunction(2)
    };
    auto bloom = std::make_shared<BloomFilter>(100, hashFuncs);
    BlacklistURL blacklist(bloom);
    BlacklistFileManager blManager("../tmp/test_blacklist.txt");
    BloomFilterFileManager bloomManager("../tmp/test_bloom.txt");

    // should return false because this in legal adding with valid url
    EXPECT_NE(simulateCommand("POST www.example.com0", blacklist, blManager, bloomManager), "400 Bad Request\n");

    // a bad request should return true 
    EXPECT_EQ(simulateCommand("POST", blacklist, blManager, bloomManager), "400 Bad Request\n");

    // using commands not exist so should return true such that be a bad request
    EXPECT_EQ(simulateCommand("RANDOM something.com", blacklist, blManager, bloomManager), "400 Bad Request\n");
}

TEST(ServerCommandTests, TestDelete) {
    auto hashFuncs = std::vector<std::shared_ptr<HashFunction>>{
        MakeHash::createHashFunction(1),
        MakeHash::createHashFunction(2)
    };
    auto bloom = std::make_shared<BloomFilter>(100, hashFuncs);
    BlacklistURL blacklist(bloom);
    BlacklistFileManager blManager("../tmp/test_blacklist.txt");
    BloomFilterFileManager bloomManager("../tmp/test_bloom.txt");

    // with an empty list, it return false because the url doesnot exist.
    EXPECT_NE(simulateCommand("DELETE www.google.com", blacklist, blManager, bloomManager), "204 No Content\n");

    // adding new url
    simulateCommand("POST test.com", blacklist, blManager, bloomManager);

    // delete an url that already in the list, so it will be return true that the delete success
    EXPECT_EQ(simulateCommand("DELETE test.com", blacklist, blManager, bloomManager), "204 No Content\n");

    // try to delete an url that doesnot exist, so it will return true for Not found
    EXPECT_EQ(simulateCommand("DELETE notfound.com", blacklist, blManager, bloomManager), "404 Not Found\n");
}

TEST(ArgumentValidationTest, ValidArguments) {
    const char* argv[] = {"./server", "12345", "100", "1", "2"};
    EXPECT_TRUE(validateArgs(5, const_cast<char**>(argv)));
}

TEST(ArgumentValidationTest, MissingArguments) {
    const char* argv[] = {"./server", "12345", "100"};
    EXPECT_FALSE(validateArgs(3, const_cast<char**>(argv)));
}

TEST(ArgumentValidationTest, InvalidPort) {
    const char* argv[] = {"./server", "-1", "100", "1"};
    EXPECT_FALSE(validateArgs(4, const_cast<char**>(argv)));
}


