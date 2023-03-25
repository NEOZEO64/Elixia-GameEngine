#include "map.hpp"

Map::Map(string ipath) : path(ipath) {

  xml_document<> doc;
  xml_node<> *mapNode;       // root node
  xml_node<> *tileSetNode;   // element node
  xml_node<> *tempLayerNode; // element node
  xml_node<> *tempDataNode;  // sub-element node
  properties = "";
  errors = "";
  tilesText = "";

  // Read the xml file into a vector
  ifstream xmlTilesFile(path);
  if (xmlTilesFile.fail()) {
    errors += fmt::format("Tile-Map file on {} doesn't exist.\n", path);
    return;
  }
  vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)),
                      istreambuf_iterator<char>());
  buffer.push_back('\0');
  doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing
                            // library into doc

  mapNode = doc.first_node("map"); // Find our root node
  tileSetNode = mapNode->first_node("tileset");

  tileSetPath = tileSetNode->first_attribute("source")->value();

  size.x = atoi(mapNode->first_attribute("width")->value());
  size.y = atoi(mapNode->first_attribute("height")->value());
  size.makeInt();
  layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

  tiles = new short[layerCount * (int)size.y * (int)size.x];

  // iterate through layers
  tempLayerNode = mapNode->first_node("layer");
  string dataDelimiter = ",";

  for (int layerID = 1; layerID <= layerCount; layerID++) {
    tempDataNode = tempLayerNode->first_node("data");
    string layerData = tempDataNode->value();
    int pos;

    // remove all \n & ' ' from layerData
    while ((pos = layerData.find('\n')) != string::npos) {
      layerData.replace(pos, 1, "");
    }
    while ((pos = layerData.find(' ')) != string::npos) {
      layerData.replace(pos, 1, "");
    }

    // needed to parse the last tileID
    layerData.append(",");

    // Parse tiles
    int layer = layerID - 1;
    int index = 0;
    while ((pos = layerData.find(dataDelimiter)) != string::npos) {
      tiles[layer * (int)size.x * (int)size.y + index] =
          stoi(layerData.substr(0, pos));
      index++;
      layerData.erase(0, pos + dataDelimiter.length());
    }
    // go to next layer
    tempLayerNode = tempLayerNode->next_sibling();
  }

  // add properties
  properties += fmt::format("Map {}:\n", path);
  properties += fmt::format("\tTileset path: {}\n", tileSetPath);
  properties += fmt::format("\tWidth: {} ,Height {}\n", size.x, size.y);
  properties += fmt::format("\tNumber of layers: {}\n", layerCount);
  properties += fmt::format("Pointer: {}\n", static_cast<void *>(this));

  tilesText += fmt::format("\tTiles:\n");
  for (short layer = 0; layer < layerCount; layer++) {
    tilesText += fmt::format("\tLayer: {}\n", layer);
    for (int y = 0; y < size.y; y++) {
      tilesText += "\t\t";
      for (int x = 0; x < 20; x++) { // you also could log all tiles if you
                                     // write "width" instead of "20"
        tilesText += fmt::format(
            "{:>3},",
            tiles[layer * (int)size.x * (int)size.y + y * (int)size.x + x]);
      }
      tilesText +=
          "...\n"; // but if you log all tiles, you have to remove these "..."
    }
  }
}

string Map::getTilesText() { return tilesText; }
string Map::getProperties() { return properties; }
string Map::getErrors() { return errors; }

short Map::getID(const Vector2D &pos, unsigned short layer) {
  return tiles[layer * (int)size.x * (int)size.y + (int)pos.y * (int)size.x +
               (int)pos.x];
}