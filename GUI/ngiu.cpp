// C++
#include <iostream>
#include <deque>
#include <string>
#include <queue>
using namespace std;
std::deque<std::string> action;
std::vector<std::string> DataBase[100000];

int find(const std::string &data)
{
    for (const auto &m : DataBase)
    {
        if (data.compare(m) == 0)
            return 1;
    }
    return 0;
}

int insert(const std::string &data)
{
    for (auto &m : DataBase)
    {
        if (data.compare(m) == 0)
            return 0;
    }
    DataBase.push_back(data);
    return 1;
}

void dataHandler(std::deque<std::string> &a)
{
    int result = 0;
    while (!a.empty())
    {
        std::string action = a.front();
        a.pop_front();
        std::string object = a.front();
        a.pop_front();
        if (action == "insert")
        {
            result = insert(object);
            std::cout << result;
        }
        if (action == "find")
        {
            result = find(object);
            std::cout << result;
        }
    }
}

int main()
{
    int result;
    std::string arrItem;
    std::string queItem;

    std::cin >> arrItem;
    while (arrItem != "*")
    {
        DataBase->push_back({arrItem});
        std::cin >> arrItem;
    }

    std::cin >> queItem;
    while (queItem != "***")
    {
        action.push_back(queItem);
        std::cin >> queItem;
    }

    action.pop_front();

    dataHandler(action);

    return 0;
}