#include "tileLib.hpp"

Map::Map(string ipath) : path(ipath) {

  xml_document<> doc;
  xml_node<> *mapNode;       // root node
  xml_node<> *tileSetNode;   // element node
  xml_node<> *tempLayerNode; // element node
  xml_node<> *tempDataNode;  // sub-element node
  properties = "";
  errors = "";

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

  // tileWidth = atoi(mapNode->first_attribute("tilewidth")->value());
  // tileHeight = atoi(mapNode->first_attribute("tileheight")->value());

  size.x = atoi(mapNode->first_attribute("width")->value());
  size.y = atoi(mapNode->first_attribute("height")->value());
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
  properties += fmt::format("\tTiles:\n");

  for (short layer = 0; layer < layerCount; layer++) {
    properties += fmt::format("\tLayer: {}\n", layer);
    for (int y = 0; y < size.y; y++) {
      properties += "\t\t";
      for (int x = 0; x < 20; x++) { // you also could log all tiles if you
                                     // write "width" instead of "20"
        properties += fmt::format(
            "{:>3},",
            tiles[layer * (int)size.x * (int)size.y + y * (int)size.x + x]);
      }
      properties +=
          "...\n"; // but if you log all tiles, you have to remove these "..."
    }
  }
}

string Map::getProperties() { return properties; }
string Map::getErrors() { return errors; }

short Map::getID(const Vector2D &pos, unsigned short layer) {
  return tiles[layer * (int)size.x * (int)size.y + (int)pos.y * (int)size.x +
               (int)pos.x];
}

void Map::render(TileSet *tileSet, unsigned short layer,
                 const Vector2D &tileRenderSize, const Vector2D &gameRenderPos,
                 const Vector2D &gameRenderSize) {

  Vector2D tempV(0, 0);
  for (tempV.y = 0; tempV.y < size.y; tempV.y++) {
    for (tempV.x = 0; tempV.x < size.x; tempV.x++) {
      tileSet->renderTile(getID(tempV, layer),
                          tempV * tileRenderSize + gameRenderPos,
                          tileRenderSize);
    }
  }
}

TileSet::TileSet(string ipath, SDL_Renderer *renderer)
    : path(ipath), renderer(renderer) {

  // Read the xml file into a vector
  ifstream xmlTilesFile(path);

  xml_document<> doc;
  xml_node<> *root_node;
  xml_node<> *element_node;

  properties = "";
  errors = "";

  if (xmlTilesFile.fail()) {
    errors += fmt::format("Tileset File on {} doesn't exist\n", path);
    return;
  }

  vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)),
                      istreambuf_iterator<char>());
  buffer.push_back('\0');
  doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing
                            // library into doc

  root_node = doc.first_node("tileset"); // Find our root node
  element_node = root_node->first_node("image");

  tileSize.x = atoi(root_node->first_attribute("tilewidth")->value());
  tileSize.y = atoi(root_node->first_attribute("tileheight")->value());
  tileCount = atoi(root_node->first_attribute("tilecount")->value());
  tileRange.x = atoi(root_node->first_attribute("columns")->value());
  tileRange.y = tileCount / tileRange.x;
  imagePath =
      string(RESOURCE_PATH) + element_node->first_attribute("source")->value();
  imagePath.insert(0, "./");

  imageSize.x = atoi(element_node->first_attribute("width")->value());
  imageSize.y = atoi(element_node->first_attribute("height")->value());

  const char *c = imagePath.c_str();
  sourceImageSet = IMG_Load(c);
  if (!sourceImageSet) {
    errors += fmt::format("Could not load tileset image on {} correctly\n",
                          imagePath);
    return;
  }
  sourceTexSet = SDL_CreateTextureFromSurface(renderer, sourceImageSet);
  SDL_FreeSurface(sourceImageSet);
  // LOG(fmt::format("Parsed tileset on {} successfully", path));

  properties += fmt::format("Tileset {}:\n", path);
  properties += fmt::format("\tImagepath: {}\n", imagePath);
  properties +=
      fmt::format("\tImage width:{} height:{}\n ", imageSize.x, imageSize.y);
  properties += fmt::format("\tTile width: {}\n", tileSize.x);
  properties += fmt::format("\tTile height: {}\n", tileSize.y);
  properties += fmt::format("\tTile columns: {}\n", tileRange.x);
  properties += fmt::format("\tTile count: {}\n", tileCount);
}

TileSet::~TileSet() { SDL_DestroyTexture(sourceTexSet); }

void TileSet::renderTest() {
  SDL_Rect tempSourceRect = {0, 0, (int)imageSize.x, (int)imageSize.y};
  SDL_Rect tempDestRect = {0, 0, (int)tileSize.x, (int)tileSize.y};
  SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
}

SDL_Rect TileSet::getRectOfID(short ID) {
  SDL_Rect tempRect = {0, 0, (int)tileSize.x, (int)tileSize.y};
  tempRect.x = ID % (int)tileRange.x - 1;
  tempRect.y = ((ID - tempRect.x) / tileRange.x);
  tempRect.x *= tileSize.x;
  tempRect.y *= tileSize.y;
  return tempRect;
}

void TileSet::renderTile(short ID, const Vector2D &pos, const Vector2D &size) {
  SDL_Rect tempSourceRect = getRectOfID(ID);
  SDL_Rect tempDestRect = {(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
  SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
}

string TileSet::getProperties() { return properties; }
string TileSet::getErrors() { return errors; }