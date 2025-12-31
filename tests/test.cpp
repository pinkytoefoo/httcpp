#include <print>
#include <string>

#include <httcpp/httcpp.h>

int main()
{
    client req;

    // sending test request
    std::println("sending request ...");
    response test = req.get("https://httpbin.org/get");
    if(test) {
        std::println("status code: {} ({})", test.http_code, (test.ok == true) ? "ok" : "not ok!");
        std::println("text:\n{}", test.text);
    }

    // getting anime data
    std::println("sending post request for anime! ...");
    std::string query = R"(
    query ($id: Int) { # Define which variables will be used in the query (id)
        Media (id: $id, type: ANIME) { # Insert our variables into the query arguments (id) (type: ANIME is hard-coded in the query)
            id
            title {
            romaji
            english
            native
            }
        }
    })";
    // TODO: make this code cleaner for users
    // i.e. abstract url queries
    std::string variables = "{id: 15125}";
    std::string data = std::format("query={}&variables={}", query, variables);
    response anime = req.post("https://graphql.anilist.co", data);
    if(anime)
    {
        std::println("status code: {} ({})", anime.http_code, (anime.ok == true) ? "ok" : "not ok!");
        std::println("text:\n{}", anime.text);
    } else {
        std::println("... oh man ...");
    }
}