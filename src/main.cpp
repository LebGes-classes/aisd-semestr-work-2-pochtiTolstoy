#include "../include/algorithm/ChanConvexHull.hpp"
#include "../include/algorithm/GrahamConvexHull.hpp"
#include "../include/algorithm/JarvisConvexHull.hpp"
#include "../include/algorithm/PointSet.hpp"
#include "../include/graphics/Button.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

class ConvexHullVisualizer {
public:
  ConvexHullVisualizer()
      : nextButton("New points", font, {20, SF_HEIGHT - 70}, {200, 50}),
        toggleMeshButton("Toggle mesh", font, {240, SF_HEIGHT - 70},
                         {200, 50}) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(SF_WIDTH, SF_HEIGHT), "Convex Hull",
                  sf::Style::Default, settings);
    window.setFramerateLimit(60);
    paddingArea.setSize(
        {SF_WIDTH - 2 * SF_PADDING, SF_HEIGHT - 2 * SF_PADDING});
    paddingArea.setPosition(SF_PADDING, SF_PADDING);
    paddingArea.setFillColor(sf::Color::Transparent);
    paddingArea.setOutlineColor(sf::Color(35, 36, 48));
    paddingArea.setOutlineThickness(2);
    if (!font.loadFromFile("../res/AgaveNerdFontMono-Bold.ttf")) {
      throw std::runtime_error("Failed to load font!");
    }
  }

  void run() {
    if (!initialize())
      return;
    mainLoop();
  }

private:
  sf::RenderWindow window;
  sf::Font font;
  Button nextButton;
  Button toggleMeshButton;
  PointSet pointSet;
  std::unique_ptr<ConvexHullBase> hullAlgorithm;
  std::vector<sf::CircleShape> points;
  std::vector<sf::Text> pointLabels;
  std::vector<sf::VertexArray> partitionMeshes;
  sf::VertexArray hullLines;
  sf::RectangleShape paddingArea;
  bool showMesh = false;
  bool showHull = true;

  bool initialize() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(SF_WIDTH, SF_HEIGHT), "Convex Hull - SFML",
                  sf::Style::Default, settings);
    window.setFramerateLimit(60);

    if (!font.loadFromFile("../res/AgaveNerdFontMono-Bold.ttf")) {
      std::cerr << "Failed to load font!" << std::endl;
      return false;
    }

    setupUI();
    generateNewSet();
    return true;
  }

  void setupUI() {
    nextButton = {"New points", font, {20, SF_HEIGHT - 70}, {200, 50}};
    toggleMeshButton = {"Toggle mesh", font, {240, SF_HEIGHT - 70}, {200, 50}};

    paddingArea.setSize(
        {SF_WIDTH - 2 * SF_PADDING, SF_HEIGHT - 2 * SF_PADDING});
    paddingArea.setPosition(SF_PADDING, SF_PADDING);
    paddingArea.setFillColor(sf::Color::Transparent);
    paddingArea.setOutlineColor(sf::Color(35, 36, 48));
    paddingArea.setOutlineThickness(2);
  }

  void generateNewSet() {
    // pointSet.generate_points(6);
    // TODO : DEBUG
    // pointSet = PointSet({{736.28, 153.004},
    //                      {1782.49, 388.164},
    //                      {992.796, 498.01},
    //                      {899.033, 841.504},
    //                      {1181.24, 803.516},
    //                      {833.415, 418.588}});
    pointSet.generate_points();
    initializeHullAlgorithm();
    createVisualElements();
  }

  void initializeHullAlgorithm() {
    hullAlgorithm = std::make_unique<ChanConvexHull>(pointSet.get_set());
    // hullAlgorithm = std::make_unique<GrahamConvexHull>(pointSet.get_set());
    // hullAlgorithm = std::make_unique<JarvisConvexHull>(pointSet.get_set());
  }

  void createVisualElements() {
    createPoints();
    createPartitionMeshes();
    createHullLines();
  }

  void createPoints() {
    points.clear();
    size_t i = 0;
    for (const auto &point : pointSet.get_set()) {
      sf::CircleShape circle(SF_POINT_RADIUS);
      circle.setPointCount(32);
      if (point == hullAlgorithm->getHull()[0]) {
        circle.setFillColor(sf::Color(0, 255, 0));
      } else {
        circle.setFillColor(sf::Color(127, 140, 170));
      }
      circle.setOutlineColor(sf::Color::Black);
      circle.setOutlineThickness(2);
      circle.setPosition(point.x_ - SF_POINT_RADIUS,
                         point.y_ - SF_POINT_RADIUS);
      points.push_back(std::move(circle));
    }
  }

  void createPartitionMeshes() {
    if (auto chanHull = dynamic_cast<ChanConvexHull *>(hullAlgorithm.get())) {
      partitionMeshes.clear();
      size_t partitions = chanHull->getPartitionsNumber();
      partitionMeshes.resize(partitions);

      for (size_t i = 0; i < partitions; ++i) {
        const auto &polygon = chanHull->getGrahamPartition(i);
        partitionMeshes[i] = sf::VertexArray(sf::LineStrip, polygon.size() + 1);

        for (size_t j = 0; j < polygon.size(); ++j) {
          partitionMeshes[i][j] = {sf::Vector2f(polygon[j].x_, polygon[j].y_),
                                   sf::Color(234, 239, 239)};
        }
        if (!polygon.empty()) {
          partitionMeshes[i][polygon.size()] = partitionMeshes[i][0];
        }
      }
    }
  }

  void createHullLines() {
    pointLabels.clear();
    const auto &hullPoints = hullAlgorithm->getHull();
    hullLines = sf::VertexArray(sf::LineStrip, hullPoints.size() + 1);

    for (size_t i = 0; i < hullPoints.size(); ++i) {
      hullLines[i] = {sf::Vector2f(hullPoints[i].x_, hullPoints[i].y_),
                      sf::Color::Green};
    }
    if (!hullPoints.empty()) {
      hullLines[hullPoints.size()] = hullLines[0];
    }

    for (size_t i = 0; i < hullPoints.size(); ++i) {
      sf::Text label;
      label.setFont(font);
      label.setCharacterSize(16);
      label.setString(std::to_string(i));
      label.setFillColor(sf::Color::White);

      // Position the label near the point
      label.setPosition(hullPoints[i].x_ + SF_POINT_RADIUS + 2,
                        hullPoints[i].y_ - SF_POINT_RADIUS);

      pointLabels.push_back(std::move(label));
    }
  }

  void handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::MouseButtonReleased &&
          event.mouseButton.button == sf::Mouse::Left) {
        handleMouseClick(event);
      }
    }
  }

  void handleMouseClick(const sf::Event &event) {
    sf::Vector2f mousePos =
        window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

    if (nextButton.contains(mousePos)) {
      generateNewSet();
    } else if (toggleMeshButton.contains(mousePos)) {
      showMesh = !showMesh;
    }
  }

  void render() {
    window.clear(sf::Color(51, 52, 70));
    window.draw(paddingArea);

    for (const auto &point : points) {
      window.draw(point);
    }

    for (const auto &label : pointLabels) {
      window.draw(label);
    }

    if (showMesh) {
      for (const auto &mesh : partitionMeshes) {
        window.draw(mesh);
      }
    }

    if (showHull) {
      window.draw(hullLines);
    }

    nextButton.draw(window);
    toggleMeshButton.draw(window);
    window.display();
  }

  void mainLoop() {
    while (window.isOpen()) {
      handleEvents();
      render();
    }
  }
};

int main() {
  ConvexHullVisualizer visualizer;
  visualizer.run();
  return EXIT_SUCCESS;
}
