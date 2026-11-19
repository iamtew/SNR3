#pragma once

#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace snr3
{

inline std::vector<int> versionParts (std::string_view s)
{
    std::vector<int> parts;
    if (! s.empty() && (s[0] == 'v' || s[0] == 'V'))
        s.remove_prefix (1);
    if (s.empty())
        return {};
    size_t i = 0;
    while (i < s.size())
    {
        if (! std::isdigit (static_cast<unsigned char> (s[i])))
            return {};
        int n = 0;
        while (i < s.size() && std::isdigit (static_cast<unsigned char> (s[i])))
            n = n * 10 + (s[i++] - '0');
        parts.push_back (n);
        if (i == s.size())
            return parts;
        if (s[i++] != '.' || i == s.size())
            return {};
    }
    return parts;
}

inline bool versionNewer (std::string_view remote, std::string_view local)
{
    auto pr = versionParts (remote);
    auto pl = versionParts (local);
    if (pr.empty() || pl.empty())
        return false;
    const size_t n = pr.size() > pl.size() ? pr.size() : pl.size();
    pr.resize (n, 0);
    pl.resize (n, 0);
    for (size_t i = 0; i < n; ++i)
    {
        if (pr[i] != pl[i])
            return pr[i] > pl[i];
    }
    return false;
}

inline std::string newestNewerTag (const std::vector<std::string>& names, std::string_view local)
{
    std::string best;
    for (const auto& name : names)
        if (versionNewer (name, local) && (best.empty() || versionNewer (name, best)))
            best = name;
    return best;
}

} // namespace snr3
