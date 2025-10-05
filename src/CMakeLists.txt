set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC
  main.cpp
  ui/MainWindow.h
  ui/MainWindow.cpp
  view/TopologyScene.h
  view/TopologyScene.cpp
  view/TopologyView.h
  view/TopologyView.cpp
  view/NodeItem.h
  view/NodeItem.cpp
  view/EdgeItem.h
  view/EdgeItem.cpp
  core/Params.h
  core/NodeEdge.h
  core/TopologyGenerator.h
  core/TopologyGenerator.cpp
  core/Params.cpp
)

add_executable(topology_diagram_generator ${SRC})

target_include_directories(topology_diagram_generator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(topology_diagram_generator PRIVATE Qt6::Widgets Qt6::Svg)


