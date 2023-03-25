#include "tileset.hpp"

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