#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>
#include <map>
#include <memory>

using namespace std;

static map<string, vector<vector<int>>> cachedMaps;

// 定义节点类
class Node {
public:
    int x, y, f, g, h;
    shared_ptr<Node> parent;

    Node(int x, int y, shared_ptr<Node> parent = nullptr) : x(x), y(y), f(0), g(INT_MAX), h(0), parent(parent) {}
};

// A*寻路算法函数
string AStar(int startX, int startY, int endX, int endY, vector<vector<int>> &map) {
    int rowSize = static_cast<int>(map.size());
    int colSize = static_cast<int>(map[0].size());
    vector<vector<bool>> closedList(rowSize, vector<bool>(colSize, false));
    vector<vector<shared_ptr<Node>>> allNodes(rowSize, vector<shared_ptr<Node>>(colSize, nullptr));

    auto comp = [](shared_ptr<Node> node1, shared_ptr<Node> node2) { return node1->f > node2->f; };
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, decltype(comp)> openList(comp);

    // 存储所有创建的节点
    vector<shared_ptr<Node>> nodes;

    shared_ptr<Node> startNode = make_shared<Node>(startX, startY);
    shared_ptr<Node> endNode = make_shared<Node>(endX, endY);
    allNodes[startX][startY] = startNode;
    openList.push(startNode);

    // 寻路主算法
    while (!openList.empty()) {
        shared_ptr<Node> currentNode = openList.top();
        openList.pop();
        closedList[currentNode->x][currentNode->y] = true;

        if (abs(currentNode->x - endNode->x) <= 1 && abs(currentNode->y - endNode->y) <= 1) {
            string path;
            while (currentNode != nullptr) {
                //由于返回的是坐标，所以将行列序号转换为坐标，即列序号y为x坐标、行序号x为y坐标
                path = to_string(currentNode->y + 1) + "," + to_string(currentNode->x + 1) + "|" + path;
                currentNode = currentNode->parent;
            }
            return path.substr(0, path.length() - 1);  // 去掉最后的"|"
        }

        // 访问周边节点
        for (int newX = max(0, currentNode->x - 1); newX <= min(rowSize - 1, currentNode->x + 1); ++newX) {
            for (int newY = max(0, currentNode->y - 1); newY <= min(colSize - 1, currentNode->y + 1); ++newY) {
                if (closedList[newX][newY] || map[newX][newY] == 1) {
                    continue;
                }
                closedList[newX][newY] = true;
                shared_ptr<Node> successor = allNodes[newX][newY];
                if (successor == nullptr) {
                    successor = make_shared<Node>(newX, newY, currentNode);
                    allNodes[newX][newY] = successor;
                }
                //int tentative_g = currentNode->g + ((newX == currentNode->x || newY == currentNode->y) ? 10 : 14);
                //对传奇来说，斜线距离和横纵向距离是一样的
                int tentative_g = currentNode->g + 1;
                if (tentative_g < successor->g) {
                    successor->parent = currentNode;
                    successor->g = tentative_g;
                    successor->h = abs(endX - newX) + abs(endY - newY);
                    successor->f = successor->g + successor->h;
                    openList.push(successor);
                }
            }
        }
    }
    return "";  // 如果没有找到路径，返回空字符串
}

// 读取地图文件
vector<vector<int>> readMapFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return {};
    }
    string line;
    vector<vector<int>> map;
    while (getline(file, line)) {
        vector<int> row;
        for (char c: line) {
            if (c == '0' || c == '1') {
                row.push_back(c - '0');
            }
        }
        map.push_back(row);
    }
    file.close();
    return map;
}

//传入的是坐标，和行号、列号是相反的，且转为容器中的索引要减1
extern "C" __declspec(dllexport) char *FindPath(const char *mapPath, int cordSX, int cordSY, int cordEX, int cordEY) {
    if (mapPath == nullptr) {
        return nullptr;
    }
    if (cachedMaps.find(mapPath) == cachedMaps.end()) {
        vector<vector<int>> map = readMapFile(mapPath);
        cachedMaps[mapPath] = map;
    }
    vector<vector<int>> map = cachedMaps[mapPath];
    string path = AStar(cordSY - 1, cordSX - 1, cordEY - 1, cordEX - 1, map);
    if (path.empty()) {
        return nullptr;
    }
    char *result = new char[path.size() + 1];
    strcpy(result, path.c_str());
    return result;
}