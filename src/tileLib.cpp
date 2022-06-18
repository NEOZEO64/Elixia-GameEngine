#include "tileLib.hpp"

Map::Map(string ipath, int iTileWidth) : path(ipath) {

  // Read the xml file into a vector
  ifstream xmlTilesFile(path);
  if (xmlTilesFile.fail()) {
    ERROR(fmt::format(
        "Tile-Map file on {} doesn't exist. This will cause errors", path));
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

  tileWidth = iTileWidth;
  tileHeight = iTileWidth;

  width = atoi(mapNode->first_attribute("width")->value());
  height = atoi(mapNode->first_attribute("height")->value());
  layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

  mapWidth = tileWidth * width;
  mapHeight = tileHeight * height;

  tiles = new short[layerCount * height * width];

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
      tiles[layer * width * height + index] = stoi(layerData.substr(0, pos));
      index++;
      layerData.erase(0, pos + dataDelimiter.length());
    }
    // go to next layer
    tempLayerNode = tempLayerNode->next_sibling();
  }
  LOG(fmt::format("Loaded map on {} successfully", path));
}

short Map::getID(int tx, int ty, int layer) {
  return tiles[layer * width * height + ty * width + tx];
}

bool Map::isColliding(TileSet *tileSet, const Vector2D &pos) {
  Vector2D tempV(((int)pos.x) / tileWidth, ((int)pos.y) / tileHeight);
  short tempID = getID(tempV.x, tempV.y, 1);
  return (0 < tempID && tempID < (tileSet->tileCount));
}

void Map::logTiles() {
  LOG("Map tiles: ");
  for (int layer = 0; layer < layerCount; layer++) {
    LOGn(fmt::format("\tLayer {}:", layer + 1));
    for (int y = 0; y < height; y++) {
      LOGn("\t");
      for (int x = 0; x < 23; x++) { // you also could log all tiles if you
                                     // write "width" instead of "23"
        LOGn(fmt::format("{:>3}",
                         tiles[layer * width * height + y * width + x]) +
             ",");
      }
      LOGn("...\n"); // but if you log all tiles, you have to remove these "..."
    }
  }
}

void Map::logProperties() {
  LOG(fmt::format("Map {}:\n", path) +
      fmt::format("\tTileset path: {}\n", tileSetPath) +
      fmt::format("\tMap columns: {} ,rows {}\n", width, height) +
      fmt::format("\tMap width: {}\n", mapWidth + mapHeight) +
      fmt::format("\tNumber of layers: {}\n", layerCount) +
      fmt::format("\tTile width: {} height {}\n", tileWidth, tileHeight));
  // logTiles();
}

void Map::render(TileSet *tileSet, int layer, const Vector2D &offset) {
  for (int ty = 0; ty < height; ty++) {
    for (int tx = 0; tx < width; tx++) {
      Vector2D tempPos(tx * tileWidth + offset.x, ty * tileWidth + offset.y);
      Vector2D tempSize(tileWidth, tileWidth);
      tileSet->renderTile(getID(tx, ty, layer), tempPos, tempSize);
    }
  }
}

TileSet::TileSet(string ipath, SDL_Renderer *renderer)
    : path(ipath), renderer(renderer) {

  // Read the xml file into a vector
  ifstream xmlTilesFile(path);
  if (xmlTilesFile.fail()) {
    ERROR(fmt::format(
        "Tileset File on {} doesn't exist. This will cause errors", path));
  }

  vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)),
                      istreambuf_iterator<char>());
  buffer.push_back('\0');
  doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing
                            // library into doc

  root_node = doc.first_node("tileset"); // Find our root node
  element_node = root_node->first_node("image");

  tileWidth = atoi(root_node->first_attribute("tilewidth")->value());
  tileHeight = atoi(root_node->first_attribute("tileheight")->value());
  tileCount = atoi(root_node->first_attribute("tilecount")->value());
  tileColumns = atoi(root_node->first_attribute("columns")->value());
  tileRows = tileCount / tileColumns;
  imagePath =
      string(RESOURCE_PATH) + element_node->first_attribute("source")->value();
  imagePath.insert(0, "./");

  imageWidth = atoi(element_node->first_attribute("width")->value());
  imageHeight = atoi(element_node->first_attribute("height")->value());

  const char *c = imagePath.c_str();
  sourceImageSet = IMG_Load(c);
  if (!sourceImageSet) {
    ERROR("Could not load tileset image on " << imagePath << " correctly");
  }
  sourceTexSet = SDL_CreateTextureFromSurface(renderer, sourceImageSet);
  SDL_FreeSurface(sourceImageSet);
  LOG(fmt::format("Parsed tileset on {} successfully", path));
}

TileSet::~TileSet() { SDL_DestroyTexture(sourceTexSet); }

void TileSet::renderTest() {
  SDL_Rect tempSourceRect = {0, 0, imageWidth, imageHeight};
  SDL_Rect tempDestRect = {0, 0, tileWidth, tileHeight};
  SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
}

SDL_Rect TileSet::getRectOfID(short ID) {
  SDL_Rect tempRect = {0, 0, tileWidth, tileHeight};

  tempRect.x = ID % tileColumns - 1;
  tempRect.y = ((ID - tempRect.x) / tileColumns);
  tempRect.x *= tileWidth;
  tempRect.y *= tileHeight;
  return tempRect;
}

void TileSet::renderTile(short ID, const Vector2D &pos, const Vector2D &size) {
  SDL_Rect tempSourceRect = getRectOfID(ID);
  SDL_Rect tempDestRect = {(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
  SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
}

void TileSet::logProperties() {
  LOG(fmt::format("Tileset {}:\n", path) +
      fmt::format("\tImagepath: {}\n", imagePath) +
      fmt::format("\tImage width:{} height:{}\n ", imageWidth, imageHeight) +
      fmt::format("\tTile width: {}\n", tileWidth) +
      fmt::format("\tTile height: {}\n", tileHeight) +
      fmt::format("\tTile columns: {}\n", tileColumns) +
      fmt::format("\tTile count: {}\n", tileCount));
}