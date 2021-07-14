#include <iostream>
#include <functional>
#include <algorithm>
#include <memory>
#include <string>

namespace {

class ContextSearching {

public:

    using StringView = std::string_view;

    virtual ~ContextSearching() = default;

    virtual typename StringView::size_type operator() (const StringView &context, const StringView &looking) const = 0;
};


class BoyerMoore : public ContextSearching {
    
    static constexpr size_t CharSetCount{ 256 };

public:

    using ContextSearching::StringView;

    virtual typename StringView::size_type operator() (const StringView &context, const StringView &looking) const override {

        if (looking.empty() || context.empty()) {
            return StringView::npos;
        }

        ptrdiff_t badCharTable[CharSetCount] = { 0 };

        std::unique_ptr<ptrdiff_t> goodCharTablePtr{ new ptrdiff_t[looking.length()] };
        ptrdiff_t* goodCharTable{ goodCharTablePtr.get() };

        this->BuildBadCharacterTable(badCharTable, looking);
        this->BuildGoodCharacterTable(goodCharTable, looking);

        auto lookingLength{ looking.length() };

        for (size_t i{ lookingLength - 1 }; i < context.length();) {
            int j{ static_cast<int>(lookingLength - 1) };

            while (j >= 0 && (context[i] == looking[j])) {
                --i, --j;
            }
            auto c1 = context[i], c2 = looking[j];
            if (j < 0) {
                return i + 1;
            }

            ptrdiff_t shift = std::max(badCharTable[context[i]], goodCharTable[j]);
            i += shift;
        }
        return StringView::npos;
    }

private:

    template<size_t N>
    static void BuildBadCharacterTable(ptrdiff_t (&badCharTable)[N], const StringView &looking) {
        auto lookingLength{ looking.length() };
        for (auto &iter : badCharTable) {
            iter = lookingLength;
        }

        for (int i = 0; i < lookingLength - 2; ++i) {
            badCharTable[looking[i]] = lookingLength - 1 - i;
        }
    }

    static bool IsPrefix(const std::string_view &context, ptrdiff_t pos) {
        size_t suffixlen{ context.length() - pos };
        for (size_t i = 0; i < suffixlen; i++) {
            if (context[i] != context[pos + i]) {
                return false;
            }
        }
        return true;
    }


    static void BuildGoodCharacterTable(ptrdiff_t *goodCharTable, const StringView &looking) {
        auto lookingLength{ looking.length() };
        size_t lastPrefixIndex{ lookingLength - 1 };
        for (int p{ static_cast<int>(lastPrefixIndex) }; p >= 0; --p) {
            if (true == IsPrefix(looking, p + 1)) {
                lastPrefixIndex = p + 1;
            }
            goodCharTable[p] = lastPrefixIndex + (lookingLength - 1 - p);
        }

        for (int p = 0; p < lookingLength - 1; ++p) {
            size_t suffixLen{ SuffixLength(looking, p) };
            if (looking[p - suffixLen] != looking[lookingLength - 1 - suffixLen]) {
                goodCharTable[lookingLength - 1 - suffixLen] = lookingLength - 1 - p + suffixLen;
            }
        }
    }


    static size_t SuffixLength(const StringView &looking, ptrdiff_t pos) {
        size_t i{ 0 };
        for (i = 0; (looking[pos - i] == looking[looking.length() - 1 - i]) && (i < pos); i++);
        return i;
    }
};

const char *text = "Here is a simple example and exxx example";

}

void Search(const std::string_view &context, const std::string_view&looking, const ContextSearching &algo = BoyerMoore{}) {
    auto data{ context.data() };
    for (size_t pos = { algo(data, looking) }; pos != ContextSearching::StringView::npos; pos = algo(data, looking)) {
        std::cout << std::string_view{ data }.substr(pos, looking.length()) << std::endl;
        data += (pos + looking.length());
    }
}

int main() {
    std::string in = "Lorem ipsum dolor sit amet, consectetur adipiscing elit,"
        " sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
    std::string needle = "pisci";
    //auto it = std::search(in.begin(), in.end(),
    //    std::boyer_moore_searcher(
    //        needle.begin(), needle.end()));
    //if (it != in.end())
    //    std::cout << "The string " << needle << " found at offset "
    //    << it - in.begin() << '\n';
    //else
    //    std::cout << "The string " << needle << " not found\n";
    const char *looking = "example";
    Search(in.c_str(), needle.data());

    return 0;
}