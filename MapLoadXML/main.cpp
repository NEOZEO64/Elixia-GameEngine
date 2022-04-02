#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"

using namespace rapidxml;
using namespace std;

string tileSetPath = "./KenneyLandscapeTiles.tsx";
string playgroundMapPath = "./PlaygroundMap.tmx";

class TileSet {
private:
  xml_document<> doc;
  xml_node<> * root_node;
  xml_node<> * element_node;

public:
  string path;
  string imagePath;
  int tileWidth, tileHeight;
  int imageWidth, imageHeight;
  int tileColumns;
  int tileCount;

  TileSet(string ipath) {
    path = ipath;
    cout << "Parsing " << path << "to TileSet Class" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
      cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }

    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 
    
    root_node = doc.first_node("tileset"); // Find our root node
    element_node = root_node->first_node("image");

    tileWidth = atoi(root_node->first_attribute("tilewidth")->value());
    tileHeight = atoi(root_node->first_attribute("tileheight")->value());
    tileCount = atoi(root_node->first_attribute("tilecount")->value());
    tileColumns = atoi(root_node->first_attribute("columns")->value());
    imagePath = element_node->first_attribute("source")->value();
    imageWidth = atoi(element_node->first_attribute("width")->value());
    imageHeight = atoi(element_node->first_attribute("height")->value());

  }

  void printProperties() {
    cout << "Tileset " << path << ":" << endl;
    cout << "Imagepath:" << imagePath << endl;
    cout << "Image W:" << imageWidth << " H:" << imageHeight << endl;
    cout << "Tile W:" << tileWidth << endl;
    cout << "Tile H:" << tileHeight << endl;
    cout << "Tile Columns:" << tileColumns << endl;
    cout << "Tile Count:" << tileCount << endl;
  }
};

class Map {
private:
  xml_document<> doc;
  xml_node<> * mapNode; //root node
  xml_node<> * tileSetNode; //element node
  xml_node<> * tempLayerNode; //element node
  xml_node<> * tempDataNode; //sub-element node

public:
  string path;
  string tileSetPath;

  int width, height;
  int layerCount;

  int tileWidth, tileHeight;

  short* tiles; // = new int[m * n];

  Map(string ipath) {
    path = ipath;
    cout << "Parsing " << path << "to Map" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
      cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }
    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 
    
    mapNode = doc.first_node("map"); // Find our root node
    tileSetNode = mapNode->first_node("tileset");

    tileSetPath = tileSetNode->first_attribute("source")->value();

    tileWidth = atoi(mapNode->first_attribute("tilewidth")->value());
    tileHeight = atoi(mapNode->first_attribute("tileheight")->value());

    width = atoi(mapNode->first_attribute("width")->value());
    height = atoi(mapNode->first_attribute("height")->value());
    layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

    tiles = new short[layerCount * height * width];

    //iterate through layers
    tempLayerNode = mapNode->first_node("layer"); 
    string dataDelimiter = ",";

    for (int layerID = 1; layerID <= layerCount; layerID ++) {
      tempDataNode = tempLayerNode->first_node("data");
      string layerData = tempDataNode->value();
      int pos;

      // remove all \n & ' ' from layerData
      while ((pos = layerData.find('\n'))!=string::npos) {
        layerData.replace(pos, 1, "");
      }
      while ((pos = layerData.find(' '))!=string::npos) {
        layerData.replace(pos, 1, "");
      }
      //needed to parse the last tileID
      layerData.append(",");
      
      //Parse tiles
      int layer = layerID-1;
      int index = 0;
      while ((pos = layerData.find(dataDelimiter)) != string::npos) {
        tiles[layer* width*height + index] = stoi(layerData.substr(0, pos));
        index ++;
        layerData.erase(0, pos + dataDelimiter.length());
      }



      //go to next layer
      tempLayerNode = tempLayerNode->next_sibling();
    }


  }

  int getID(int x, int y, int layer) {
    return tiles[layer* width*height + y * width + x];
  }

  void printTiles() {
    for (int layer = 0; layer<layerCount; layer++) {
      cout << "Layer " << layer+1 << ":" << endl;
      for (int y = 0; y<height; y++) {
        for (int x = 0; x<width; x++) {
          cout << tiles[layer* width*height + y * width + x] << ",";
        }
        cout << endl;
      }
    }
  }

  void printProperties() {
    cout << "Map " << path << ":" << endl;
    cout << "Tileset Path:" << tileSetPath << endl;
    cout << "Map W:" << width << " H:" << height << endl;
    cout << "Number of layers: " << layerCount << endl;
    cout << "Tile W:" << tileWidth << " H:" << tileHeight << endl;
  }
};

int main(void) {
  cout << endl << endl << endl;

  TileSet tileSet(tileSetPath);
  tileSet.printProperties();

  Map playgroundMap(playgroundMapPath);
  //playgroundMap.printProperties();
  playgroundMap.printTiles();



  return 0;
}