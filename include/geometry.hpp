//
// Created by dgmuller on 5/29/25.
//

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

namespace farcical {
  enum class Orientation {
    Horizontal,
    Vertical
  };

  struct Rectangle {
    enum class Corner {
      TopLeft       = 0,
      TopRight      = 1,
      BottomLeft    = 2,
      BottomRight   = 3,
      NumCorners    = 4
    };

    enum class Edge {
      Left          = 0,
      Right         = 1,
      Top           = 2,
      Bottom        = 3,
      NumEdges      = 4
    };
  };

}

#endif //GEOMETRY_HPP
