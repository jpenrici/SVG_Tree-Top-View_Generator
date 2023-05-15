#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include <vector>

constexpr auto PI = 3.1415926;

inline double Rad(double angle) {
    return angle * PI / 180;
}

inline double Cos(double value, double radius, int angle)
{
    return value + radius * cos(Rad(angle));
}

inline double Sin(double value, double radius, int angle)
{
    return value + radius * sin(Rad(angle));
}

inline double Distance(double x0, double y0, double x1, double y1)
{
    return sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
}

inline int LineAngle(double x0, double y0, double x1, double y1)
{
    int angle = atan((y1 - y0) / (x1 - x0)) * 180.0 / PI;
    if (x0 >  x1 && y0 == y1) { angle = 0; }
    if (x0 == x1 && y0 <  y1) { angle = 90; }
    if (x0 <  x1 && y0 == y1) { angle = 180; }
    if (x0 == x1 && y0 <  y1) { angle = 270; }
    if (x0 <  x1 && y0 >  y1) { angle += 180; }
    if (x0 <  x1 && y0 <  y1) { angle += 180; }
    if (x0 >  x1 && y0 <  y1) { angle += 360; }
    return angle;
}

class SVG {

public:

    struct Metadata {
        std::string creator = "SVG tree image in top view created automatically by algorithm in C++.";
        std::string title = "SVG Tree Top View";
        std::string publisherAgentTitle = "";
        std::string date = "";

        Metadata(){};
        Metadata(std::string creator, std::string title, std::string date, std::string publisher)
            : creator(creator), title(title), date(date), publisherAgentTitle(publisher) {}
    };

    struct Point {

        double x = 0.0;
        double y = 0.0;

        Point(double x, double y) : x(x), y(y) {};
        std::string toStr() { return std::to_string(x) + "," + std::to_string(y); }
    };

    struct Shape {
        std::string name;
        std::string fill, stroke;
        double strokeWidth;
        std::vector<Point> points;

        Shape(std::string name,  std::string fill, std::string stroke, double strokeWidth)
            : name(name), fill(fill), stroke(stroke), strokeWidth(strokeWidth), points({}) {}
        Shape(std::string name,  std::string fill, std::string stroke, double strokeWidth, std::vector<Point> points)
            : name(name), fill(fill), stroke(stroke), strokeWidth(strokeWidth), points(points) {}
    };

    static const std::string RGB2HEX(unsigned R, unsigned G, unsigned B) {
        auto int2hex = [](unsigned value) {
            std::string digits = "0123456789ABCDEF";
            std::string result = "";
            if (value < 16) {
                result.push_back('0');
                result.push_back(digits[value % 16]);
            } else {
                while (value != 0) {
                    result = digits[value % 16] + result;
                    value /= 16;
                }
            }
            return result;
        };

        return "#" + int2hex(R) + int2hex(G) + int2hex(B);
    }

    static const std::string svg(int width, int height, const std::string& figure,
                                 Metadata metadata)
    {
        std::string now = "";
        try {
            std::time_t t = std::time(nullptr);
            std::tm *const pTm = std::localtime(&t);
            now = std::to_string(1900 + + pTm->tm_year);
        } catch (...) {
           // pass
        }

        metadata.date = metadata.date == "" ? now : metadata.date;

        return {
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
            "<svg\n"
            "   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
            "   xmlns:cc=\"http://creativecommons.org/ns#\"\n"
            "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
            "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
            "   xmlns=\"http://www.w3.org/2000/svg\"\n"
            "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
            "   width=\"" + std::to_string(width) + "\"\n"
            "   height=\"" + std::to_string(height) + "\"\n"
            "   viewBox= \"0 0 " + std::to_string(width) + " " + std::to_string(height) + "\"\n"
            "   version=\"1.1\"\n"
            "   id=\"svg8\">\n"
            "  <title\n"
            "     id=\"title1\">" + metadata.title + "</title>\n"
            "  <defs\n"
            "     id=\"defs1\" />\n"
            "  <metadata\n"
            "     id=\"metadata1\">\n"
            "    <rdf:RDF>\n"
            "      <cc:Work\n"
            "         rdf:about=\"\">\n"
            "        <dc:format>image/svg+xml</dc:format>\n"
            "        <dc:type\n"
            "           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n"
            "        <dc:title>" + metadata.title + "</dc:title>\n"
            "        <dc:date>" + metadata.date + "</dc:date>\n"
            "        <dc:publisher>\n"
            "          <cc:Agent>\n"
            "            <dc:title>" + metadata.publisherAgentTitle + "</dc:title>\n"
            "          </cc:Agent>\n"
            "        </dc:publisher>\n"
            "        <dc:subject>\n"
            "          <rdf:Bag>\n"
            "            <rdf:li>tree</rdf:li>\n"
            "            <rdf:li>plant</rdf:li>\n"
            "            <rdf:li>nature</rdf:li>\n"
            "            <rdf:li>landscaping</rdf:li>\n"
            "          </rdf:Bag>\n"
            "        </dc:subject>\n"
            "        <dc:creator>\n"
            "          <cc:Agent>\n"
            "            <dc:title>" + metadata.creator + "</dc:title>\n"
            "          </cc:Agent>\n"
            "        </dc:creator>\n"
            "        <cc:license\n"
            "           rdf:resource=\"http://creativecommons.org/publicdomain/zero/1.0/\" />\n"
            "        <dc:description>SVG tree image in top view created automatically by algorithm in C++.</dc:description>\n"
            "      </cc:Work>\n"
            "      <cc:License\n"
            "         rdf:about=\"http://creativecommons.org/publicdomain/zero/1.0/\">\n"
            "        <cc:permits\n"
            "           rdf:resource=\"http://creativecommons.org/ns#Reproduction\" />\n"
            "        <cc:permits\n"
            "           rdf:resource=\"http://creativecommons.org/ns#Distribution\" />\n"
            "        <cc:permits\n"
            "           rdf:resource=\"http://creativecommons.org/ns#DerivativeWorks\" />\n"
            "      </cc:License>\n"
            "    </rdf:RDF>\n"
            "  </metadata>\n"
            "  <!--      Created in C++ algorithm       -->\n"
            "  <!-- Attention: do not modify this code. -->\n"
            "\n"
            "" + figure + ""
            "\n"
            "  <!-- Attention: do not modify this code. -->\n"
            "</svg>"
        };
    }

    static const std::string polyline(Shape shape) {

        if (shape.points.empty()) {
            return "<!-- Empty -->\n";
        }

        std::string values = "";
        for (unsigned i = 0; i < shape.points.size(); i++) {
            values += shape.points[i].toStr() + " ";
        }
        shape.name = shape.name.empty() ? "polyline" : shape.name;
        shape.stroke = shape.stroke.empty() ? "#000000" : shape.stroke;
        std::string strokeWidth = std::to_string(shape.strokeWidth);
        return {
            "     <polyline\n"
            "        id=\"" + shape.name + "\"\n"
            "        style=\"opacity:1.0;fill:none;stroke:" + shape.stroke + ";stroke-width:" + strokeWidth + ";stroke-opacity:1\"\n"
            "        points=\"" + values + "\" />\n"
        };
    }

    static const std::string polygon(Shape shape) {

        if (shape.points.empty()) {
            return "<!-- Empty -->\n";
        }

        std::string values = "";
        for (unsigned i = 0; i < shape.points.size() - 1; i++) {
            values += shape.points[i].toStr() + " L ";
        }
        values += shape.points[shape.points.size() - 1].toStr();
        shape.name = shape.name.empty() ? "polygon" : shape.name;
        shape.fill = shape.fill.empty() ? "#FFFFFF" : shape.fill;
        shape.stroke = shape.stroke.empty() ? "#000000" : shape.stroke;
        std::string strokeWidth = std::to_string(shape.strokeWidth);
        return {
            "     <path\n"
            "        id=\"" + shape.name + "\"\n"
            "        style=\"opacity:1.0;fill:" + shape.fill + ";stroke:" + shape.stroke + ";stroke-width:" + strokeWidth + ";stroke-opacity:1\"\n"
            "        d=\"M " + values + " Z\" />\n"
        };
    }

    static bool save(const std::string& text, std::string path = "")
    {
        if (path.empty()) {
            path = "svgOut.txt";
        }

        try {
            std::ofstream file(path, std::ios::out);
            file << text;
            file.close();
        }
        catch (const std::exception& e) {
            std::cout << "Error handling file writing.\n";
            std::cerr << e.what() << "\n";
            return false;
        }

        return true;
    }
};
