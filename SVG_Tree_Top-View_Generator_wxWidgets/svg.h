#pragma once

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

constexpr auto PI = 3.1415926;

inline auto Rad(const double &angle) -> double
{
    return angle * PI / 180;
}

inline auto Cos(const double &value, const double &radius, const int &angle) -> double
{
    return value + radius * std::cos(Rad(angle));
}

inline auto Sin(const double &value, const double &radius, const double &angle) -> double
{
    return value + radius * std::sin(Rad(angle));
}

inline auto Distance(const double &x0, const double &y0, const double &x1, const double &y1) -> double
{
    return std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
}

inline auto LineAngle(const double &x0, const double &y0, const double &x1, const double &y1) -> int
{
    int angle = static_cast<int>(std::atan((y1 - y0) / (x1 - x0)) * 180.0 / PI);
    if (x0 >  x1 && y0 == y1) {
        angle = 0;
    }
    if (x0 == x1 && y0 <  y1) {
        angle = 90;
    }
    if (x0 <  x1 && y0 == y1) {
        angle = 180;
    }
    if (x0 == x1 && y0 <  y1) {
        angle = 270;
    }
    if (x0 <  x1 && y0 >  y1) {
        angle += 180;
    }
    if (x0 <  x1 && y0 <  y1) {
        angle += 180;
    }
    if (x0 >  x1 && y0 <  y1) {
        angle += 360;
    }
    return angle;
}

class SVG {

public:

    struct Metadata {
        std::string creator = "SVG tree image in top view created automatically by algorithm in C++.";
        std::string title = "SVG Tree Top View";
        std::string publisherAgentTitle;
        std::string date;

        Metadata() = default;
        Metadata(std::string creator, std::string title, std::string publisher)
            : creator(std::move(creator)), title(std::move(title)), publisherAgentTitle(std::move(publisher)) {}
    };

    struct Point {

        double x = 0.0;
        double y = 0.0;

        Point(const double &x, const double &y) : x(x), y(y) {};

        [[nodiscard]] auto toStr() const -> std::string
        {
            return std::to_string(x) + "," + std::to_string(y);
        }
    };

    struct Shape {
        std::string name;
        std::string fill, stroke;
        double strokeWidth;
        std::vector<Point> points;

        Shape(std::string name,  std::string fill, std::string stroke, double strokeWidth)
            : name(std::move(name)), fill(std::move(fill)), stroke(std::move(stroke)), strokeWidth(strokeWidth),
              points({}) {}
        Shape(std::string name,  std::string fill, std::string stroke, double strokeWidth,
              std::vector<Point> points)
            : name(std::move(name)), fill(std::move(fill)), stroke(std::move(stroke)), strokeWidth(strokeWidth),
              points(std::move(points)) {}
    };

    static auto RGB2HEX(const unsigned &R, const unsigned &G, const unsigned &B)  -> std::string
    {
        auto int2hex = [](unsigned value) {
            std::string digits = "0123456789ABCDEF";
            std::string result;
            if (value < 16) {
                result.push_back('0');
                result.push_back(digits[value % 16]);
            }
            else {
                while (value != 0) {
                    result = digits[value % 16] + result;
                    value /= 16;
                }
            }
            return result;
        };

        return "#" + int2hex(R) + int2hex(G) + int2hex(B);
    }

    static auto svg(const int &width, const int &height, const std::string &figure,
                    Metadata metadata) -> std::string
    {
        std::string now;
        try {
            std::time_t t = std::time(nullptr);
            std::tm *const pTm = std::localtime(&t);
            now = std::to_string(1900 + + pTm->tm_year);
        }
        catch (...) {
            // pass
        }

        metadata.date = metadata.date.empty() ? now : metadata.date;

        return {
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
            "<svg\n"
            "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
            "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
            "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
            "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
            "xmlns=\"http://www.w3.org/2000/svg\"\n"
            "xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
            "width=\"" + std::to_string(width) + "\"\n"
            "height=\"" + std::to_string(height) + "\"\n"
            "viewBox= \"0 0 " + std::to_string(width) + " " + std::to_string(height) + "\"\n"
            "version=\"1.1\"\n"
            "id=\"svg8\">\n"
            "<title\n"
            "id=\"title1\">" + metadata.title + "</title>\n"
            "<defs\n"
            "id=\"defs1\" />\n"
            "<metadata\n"
            "id=\"metadata1\">\n"
            "<rdf:RDF>\n"
            "<cc:Work\n"
            "rdf:about=\"\">\n"
            "<dc:format>image/svg+xml</dc:format>\n"
            "<dc:type\n"
            "rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n"
            "<dc:title>" + metadata.title + "</dc:title>\n"
            "<dc:date>" + metadata.date + "</dc:date>\n"
            "<dc:publisher>\n"
            "<cc:Agent>\n"
            "<dc:title>" + metadata.publisherAgentTitle + "</dc:title>\n"
            "</cc:Agent>\n"
            "</dc:publisher>\n"
            "<dc:subject>\n"
            "<rdf:Bag>\n"
            "<rdf:li>tree</rdf:li>\n"
            "<rdf:li>plant</rdf:li>\n"
            "<rdf:li>nature</rdf:li>\n"
            "<rdf:li>landscaping</rdf:li>\n"
            "</rdf:Bag>\n"
            "</dc:subject>\n"
            "<dc:creator>\n"
            "<cc:Agent>\n"
            "<dc:title>" + metadata.creator + "</dc:title>\n"
            "</cc:Agent>\n"
            "</dc:creator>\n"
            "<cc:license\n"
            "rdf:resource=\"http://creativecommons.org/publicdomain/zero/1.0/\" />\n"
            "<dc:description>SVG tree image in top view created automatically by algorithm in C++."
            "Developer: https://github.com/jpenrici</dc:description>\n"
            "</cc:Work>\n"
            "<cc:License\n"
            "rdf:about=\"http://creativecommons.org/publicdomain/zero/1.0/\">\n"
            "<cc:permits\n"
            "rdf:resource=\"http://creativecommons.org/ns#Reproduction\" />\n"
            "<cc:permits\n"
            "rdf:resource=\"http://creativecommons.org/ns#Distribution\" />\n"
            "<cc:permits\n"
            "rdf:resource=\"http://creativecommons.org/ns#DerivativeWorks\" />\n"
            "</cc:License>\n"
            "</rdf:RDF>\n"
            "</metadata>\n"
            "<!--      Created in C++ algorithm       -->\n"
            "<!-- Attention: do not modify this code. -->\n"
            "\n" + figure + "\n"
            "  <!-- Attention: do not modify this code. -->\n"
            "</svg>"
        };
    }

private:

    static auto rtrimZeros(const std::string &str) -> std::string
    {
        auto right = str.size() - 1;
        while (right >= 0) {
            if (str[right] != '0') {
                break;
            }
            right--;
        }

        return str.substr(0, 1 + right) + '0';
    }

    // Validates and formats entries.
    static auto style(std::string name, std::string fill, std::string stroke, double strokeWidth,
                      double fillOpacity, double strokeOpacity) -> std::string
    {
        name   = name.empty() ? "Shape" : name;
        fill   = fill.empty() ? "#FFFFFF" : fill;
        stroke = stroke.empty() ? "#000000" : stroke;
        fillOpacity   = fillOpacity < 0 ? 0 : std::min(fillOpacity / 255, 1.0);
        strokeOpacity = strokeOpacity < 0 ? 0 : std::min(strokeOpacity / 255, 1.0);

        return {
            "id=\"" + name + "\"\nstyle=\"" +
            "opacity:" + rtrimZeros(std::to_string(fillOpacity)) + ";fill:" + fill +
            ";stroke:" + stroke + ";stroke-width:" + rtrimZeros(std::to_string(strokeWidth)) +
            ";stroke-opacity:" + rtrimZeros(std::to_string(strokeOpacity)) +
            ";stroke-linejoin:round;stroke-linecap:round\"\n" };
    }

public:

    static auto group(std::string id, const std::string &elements) -> std::string
    {
        id = id.empty() ? "<g>\n" : "<g id=\"" + id + "\" >\n";
        return elements.empty() ? "" : id + elements + "</g>\n";
    }

    static auto polyline(Shape shape) -> std::string
    {

        if (shape.points.empty()) {
            return "<!-- Empty -->\n";
        }

        std::string values;
        for (auto &point : shape.points) {
            values += point.toStr() + " ";
        }

        shape.name = shape.name.empty() ? "polyline" : shape.name;
        return {
            "<polyline\n" + style(shape.name, "none", shape.stroke, shape.strokeWidth, 255, 255) +
            "points=\"" + values + "\" />\n"
        };
    }

    static auto polygon(Shape shape) -> std::string
    {

        if (shape.points.empty()) {
            return "<!-- Empty -->\n";
        }

        std::string values;
        for (unsigned i = 0; i < shape.points.size() - 1; i++) {
            values += shape.points[i].toStr() + " L ";
        }
        values += shape.points[shape.points.size() - 1].toStr();

        shape.name = shape.name.empty() ? "polygon" : shape.name;
        return {
            "<path\n" + style(shape.name, shape.fill, shape.stroke, shape.strokeWidth, 255, 255) +
            "d=\"M " + values + " Z\" />\n"
        };
    }

    static auto save(const std::string &text, std::string path = "") -> bool
    {
        if (path.empty()) {
            path = "svgOut.txt";
        }

        try {
            std::ofstream file(path, std::ios::out);
            file << text;
            file.close();
        }
        catch (const std::exception &e) {
            std::cout << "Error handling file writing.\n";
            std::cerr << e.what() << "\n";
            return false;
        }

        return true;
    }
};
