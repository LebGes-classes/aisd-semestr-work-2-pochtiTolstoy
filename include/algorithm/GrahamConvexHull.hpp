#pragma once

#include "ConvexHullBase.hpp"
#include "Point.hpp"
#include <cassert>
#include <vector>

class GrahamConvexHull : public ConvexHullBase {
public:
  class Vector {
  public:
    double x, y;

    Vector() : x{0}, y{0} {}

    Vector(const Point &p1, const Point &p2) {
      this->x = p2.x_ - p1.x_;
      this->y = p2.y_ - p1.y_;
    }

    static int cross(const Vector &v1, const Vector &v2) {
      return v1.x * v2.y - v1.y * v2.x;
    }
  };

  GrahamConvexHull(const std::vector<Point> &set) : ConvexHullBase(set) {
    computeHull();
  }

  const std::vector<Point> &getHull() const override { return hull_; }

  size_t size() const override { return hull_.size(); }

private:
  void sortPoints(std::vector<Point> &vect, Point centralP) {
    int j = 0;
    Point temp;
    Vector v1;
    Vector v2;

    for (int i = 1; i < vect.size(); i++) {

      v1 = Vector(centralP, vect[i - 1]);
      v2 = Vector(centralP, vect[i]);

      j = i;
      while (j > 0 && Vector::cross(v1, v2) < 0) {
        temp = vect[j - 1];
        vect[j - 1] = vect[j];
        vect[j] = temp;
        j--;

        if (j > 0) {
          v1 = Vector(centralP, vect[j - 1]);
          v2 = Vector(centralP, vect[j]);
        }
      }
    }
  }

  void computeHull() override {
    Point lowestP = points_[0];
    Point currP;

    for (int i = 1; i < points_.size(); i++) {
      currP = points_[i];

      if (lowestP.y_ > currP.y_ ||
          (lowestP.y_ == currP.y_ && lowestP.x_ > currP.x_)) {
        lowestP = currP;
      }
    }

    std::vector<Point> vc;

    for (int i = 0; i < points_.size(); i++) {
      if (points_[i] != lowestP) {
        vc.push_back(points_[i]);
      }
    }

    sortPoints(vc, lowestP);

    std::vector<Point> result;

    result.push_back(lowestP);
    result.push_back(vc[0]);

    Vector v1; // вектор который связывает точку предпоследнюю в стеке и
               // нынешнюю точку в итерации
    Vector v2; // вектор который связывает точку предпоследнюю и последнюю точку
               // в стеке
    Point p1;  // предпоследняя точка
    Point p2;  // последняя точка
    Point p3;  // нынешняя точка в итерации

    for (int i = 1; i < vc.size(); i++) {
      p3 = vc[i];

      p2 = result.back();
      result.pop_back();
      p1 = result.back();

      v1 = Vector(p1, p3);
      v2 = Vector(p1, p2);

      int j = i;
      while (Vector::cross(v2, v1) < 0) {
        // std::cout << "deleted point: " << std::endl
        //           << "x:" << p2.x_ << std::endl
        //           << "y: " << p2.y_ << std::endl;
        result.push_back(p3);

        p3 = result.back();
        result.pop_back();

        p2 = result.back();
        result.pop_back();

        p1 = result.back();

        v1 = Vector(p1, p3);
        v2 = Vector(p1, p2);
      }

      result.push_back(p2);
      result.push_back(p3);
    }

    this->hull_ = result;
    assert(hull_.size() <= points_.size());
  }
};

// points.push_back(Point(3, 2));
// points.push_back(Point(2, 6));
// points.push_back(Point(5, 5));
// points.push_back(Point(5, 3));
// points.push_back(Point(7, 3));
// points.push_back(Point(7, 7));
// points.push_back(Point(9, 6));
// points.push_back(Point(7, 9));
// points.push_back(Point(11, 7));
// points.push_back(Point(10, 2));
