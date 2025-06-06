#include <iostream>
#include <cmath> 
#include <vector>
#include <stack>
 //Point ясен хуй точка
 //Vector вектор с двумя координатами
 //sortPoints сортирует список точек относительно данной точки по полярным углам
 //grahamScan формирует МВО
 

class Point{

public:
    int x, y;

    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
 };

void showVector(std::vector<Point*> &vect){

    for(int i = 0; i < vect.size(); i++){

        std::cout << "x: " << vect[i]->x << "; y: " << vect[i]->y << std::endl;
    }
}


void sortPoints(std::vector<Point*> &vect, Point* centralP);

class Vector{

public:
    int x, y;

    Vector(Point* p1, Point* p2){
        this-> x = p2->x - p1->x;
        this-> y = p2->y - p1->y;
    }

    static int cross(Vector* v1, Vector* v2){

        return v1->x * v2->y - v1->y * v2->x;
    }
 };

void sortPoints(std::vector<Point*> &vect, Point* centralP){
    int j = 0;
    Point* temp = NULL;
    Vector* v1 = NULL;
    Vector* v2 = NULL;


    for(int i = 1; i < vect.size(); i++){  
        
        v1 = new Vector(centralP, vect[i-1]);
        v2 = new Vector(centralP, vect[i]);

        j = i;  
        while(j > 0 && Vector::cross(v1, v2) < 0){
            
            temp = vect[j - 1];
            vect[j - 1] = vect[j];
            vect[j] = temp;


            j--;
            delete v1;
            delete v2;
            
            if(j > 0){
                v1 = new Vector(centralP, vect[j-1]);
                v2 = new Vector(centralP, vect[j]);
            }
        }

    }


}

std::vector<Point*>& grahamScan(std::vector<Point*> &vect){
    
    Point* lowestP = vect[0];
    Point* currP;

    for(int i = 1; i < vect.size(); i++){
        currP = vect[i];

        if(lowestP->y > currP->y || (lowestP->y == currP->y && lowestP->x > currP->x)){
            lowestP = currP;
        }
    }
    
    std::vector<Point*> vc;
    
    for(int i = 0; i < vect.size(); i++){

        if(vect[i] != lowestP){
            vc.push_back(vect[i]);
            
        }
    }

    sortPoints(vc, lowestP);
    
    std::vector<Point*> result;

    result.push_back(lowestP);
    result.push_back(vc[0]);
    
    Vector* v1 = NULL;//вектор который связывает точку предпоследнюю в стеке и нынешнюю точку в итерации
    Vector* v2 = NULL;//вектор который связывает точку предпоследнюю и последнюю точку в стеке

    Point* p1 = NULL;//предпоследняя точка
    Point* p2 = NULL;//последняя точка
    Point* p3 = NULL;//нынешняя точка в итерации

    for(int i = 1; i < vc.size(); i++){
        p3 = vc[i];

        p2 = result.back();
        result.pop_back();
        p1 = result.back();
        
        v1 = new Vector(p1, p3);
        v2 = new Vector(p1, p2);

        int j = i;
        while(Vector::cross(v2, v1) < 0){
            std::cout << "deleted point: " << std::endl << "x:" << p2->x << std::endl << "y: " << p2->y << std::endl;
            result.push_back(p3);

            p3 = result.back();
            result.pop_back();

            p2 = result.back();
            result.pop_back();

            p1 = result.back();

            delete v2;
            delete v1;

            v1 = new Vector(p1, p3);
            v2 = new Vector(p1, p2);
        }
        delete v2;
        delete v1;

        result.push_back(p2);
        result.push_back(p3); 

        delete v1;
        delete v2;
    }
    showVector(result);
    
    std::vector<Point*>& res = result;
    return res;

}


 int main(){
    
    std::vector<Point*> points;

    points.push_back(new Point(3, 2));
    points.push_back(new Point(2, 6));
    points.push_back(new Point(5, 5));
    points.push_back(new Point(5, 3));
    points.push_back(new Point(7, 3));
    points.push_back(new Point(7, 7));
    points.push_back(new Point(9, 6));
    points.push_back(new Point(7, 9));
    points.push_back(new Point(11, 7));
    points.push_back(new Point(10, 2));
    
    std::vector<Point*>& result = grahamScan(points);
    showVector(result);

    return 0;
 }

