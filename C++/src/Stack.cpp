#include "Stack.hpp"

#include <sstream>
#include <string>

namespace lab3 {

bool Stack::peek(std::string& value) const {
    if (data_.empty()) {
        return false;
    }
    return data_.getAt(data_.size() - 1, value);
}

std::string Stack::toString() const {
    if (data_.empty()) {
        return "Стек пуст";
    }
    std::ostringstream out;
    out << "вершина -> ";
    for (std::size_t i = data_.size(); i > 0; --i) {
        std::string value;
        data_.getAt(i - 1, value);
        out << value;
        if (i != 1) {
            out << ' ';
        }
    }
    out << " <- дно";
    return out.str();
}

}  // namespace lab3
