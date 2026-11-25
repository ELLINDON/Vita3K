// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <util/log.h>
#include <util/net_utils.h>

#include <curl/curl.h>

#ifdef _WIN32
#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <algorithm>

#include <condition_variable>
#include <mutex>

namespace net_utils {

// 0 is ok, negative is bad
SceHttpErrorCode parse_url(const std::string &url, parsedUrl &out) {
    out.scheme = url.substr(0, url.find(':'));

    if (out.scheme != "http" && out.scheme != "https")
        return SCE_HTTP_ERROR_UNKNOWN_SCHEME;

    // Check if URL is opaque, if it is opaque then its invalid
    // http://
    //+    012
    //      ^^ Check these 2
    char url1Slash = *(url.c_str() + (out.scheme.length() + 1)); // get uint8 value
    char url2Slash = *(url.c_str() + (out.scheme.length() + 2)); // get next uint8 value
    // Compare the 2 characters that are supposed to be slahses
    if (url1Slash != '/' || url2Slash != '/') {
        out.invalid = true;
        return (SceHttpErrorCode)0;
    }

    auto end_scheme_pos = url.find(':');
    auto has_path = url.find('/', end_scheme_pos + 3) != std::string::npos;

    auto full_wo_scheme = url.substr(end_scheme_pos + 3);

    if (has_path) {
        // username:password@lttstore.com:727/wysi/cookie.php?pog=gers#extremeexploit

        {
            auto path_pos = std::string(full_wo_scheme).find('/');
            auto full_no_scheme_path = full_wo_scheme.substr(0, path_pos);
            // full_no_scheme_path = username:password@lttstore.com:727
            auto c = full_no_scheme_path.find('@');
            if (c != std::string::npos) { // we have credentials
                auto credentials = full_no_scheme_path.substr(0, c);
                // credentials = username:password
                auto semicolon_pos = credentials.find(':');

                if (semicolon_pos != std::string::npos) { // we have password?
                    auto username = credentials.substr(0, semicolon_pos);
                    auto password = credentials.substr(semicolon_pos + 1);

                    if (username.length() >= SCE_HTTP_USERNAME_MAX_SIZE)
                        return SCE_HTTP_ERROR_OUT_OF_SIZE;
                    if (password.length() >= SCE_HTTP_USERNAME_MAX_SIZE)
                        return SCE_HTTP_ERROR_OUT_OF_SIZE;

                    out.username = username;
                    out.password = password;
                } else { // we don't have password
                    out.username = credentials;
                }
            } else { // no credentials
                // lttstore.com:727
                auto semicolon_pos = full_no_scheme_path.find(':');

                if (semicolon_pos != std::string::npos) { // we have port
                    auto hostname = full_no_scheme_path.substr(0, semicolon_pos);
                    auto port = full_no_scheme_path.substr(semicolon_pos + 1);

                    out.hostname = hostname;
                    out.port = port;
                } else { // no port
                    out.hostname = full_no_scheme_path;
                }
            }
        }
        {
            auto path_pos = std::string(full_wo_scheme).find('/');
            auto full_no_scheme_hostname = full_wo_scheme.substr(path_pos);
            // /wysi/cookie.php?pog=gers#extremeexploit

            auto query_pos = full_no_scheme_hostname.find('?');
            if (query_pos != std::string::npos) { // we have query
                auto path = full_no_scheme_hostname.substr(0, query_pos);
                out.path = path;

                auto query_frag = full_no_scheme_hostname.substr(query_pos);

                auto frag_pos = query_frag.find('#');
                if (frag_pos != std::string::npos) {
                    // query and fragment

                    auto query = query_frag.substr(0, frag_pos);
                    auto fragment = query_frag.substr(frag_pos);

                    out.query = query;
                    out.fragment = fragment;
                } else { // no fragment
                    out.query = query_frag;
                }

            } else { // no query, dunno about fragment
                auto frag_pos = full_no_scheme_hostname.find('#');

                if (frag_pos != std::string::npos) { // we have fragment
                    auto path = full_no_scheme_hostname.substr(0, frag_pos);
                    auto fragment = full_no_scheme_hostname.substr(frag_pos);

                    out.path = path;
                    out.fragment = fragment;
                } else { // no fragment, only path
                    out.path = full_no_scheme_hostname;
                }
            }
        }
    } else {
        // username:password@lttstore.com:727
        auto c = full_wo_scheme.find('@');
        if (c != std::string::npos) { // we have credentials
            auto credentials = full_wo_scheme.substr(0, c);
            // credentials = username:password
            auto semicolon_pos = credentials.find(':');

            if (semicolon_pos != std::string::npos) { // we have password?
                auto username = credentials.substr(0, semicolon_pos);
                auto password = credentials.substr(semicolon_pos + 1);

                if (username.length() >= SCE_HTTP_USERNAME_MAX_SIZE)
                    return SCE_HTTP_ERROR_OUT_OF_SIZE;
                if (password.length() >= SCE_HTTP_USERNAME_MAX_SIZE)
                    return SCE_HTTP_ERROR_OUT_OF_SIZE;

                out.username = username;
                out.password = password;
            } else { // we don't have password
                out.username = credentials;
            }
        } else { // no credentials
            // lttstore.com:727
            auto semicolon_pos = full_wo_scheme.find(':');

            if (semicolon_pos != std::string::npos) { // we have port
                auto hostname = full_wo_scheme.substr(0, semicolon_pos);
                auto port = full_wo_scheme.substr(semicolon_pos + 1);

                out.hostname = hostname;
                out.port = port;
            } else { // no port
                out.hostname = full_wo_scheme;
            }
        }
    }

    return (SceHttpErrorCode)0;
}

int char_method_to_int(const char *method) {
    if (strcmp(method, "GET") == 0) {
        return SCE_HTTP_METHOD_GET;
    } else if (strcmp(method, "POST") == 0) {
        return SCE_HTTP_METHOD_POST;
    } else if (strcmp(method, "HEAD") == 0) {
        return SCE_HTTP_METHOD_HEAD;
    } else if (strcmp(method, "OPTIONS") == 0) {
        return SCE_HTTP_METHOD_OPTIONS;
    } else if (strcmp(method, "PUT") == 0) {
        return SCE_HTTP_METHOD_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return SCE_HTTP_METHOD_DELETE;
    } else if (strcmp(method, "TRACE") == 0) {
        return SCE_HTTP_METHOD_TRACE;
    } else if (strcmp(method, "CONNECT") == 0) {
        return SCE_HTTP_METHOD_CONNECT;
    } else {
        return -1;
    }
}

const char *int_method_to_char(const int n) {
    switch (n) {
    case 0: return "GET";
    case 1: return "POST";
    case 2: return "HEAD";
    case 3: return "OPTIONS";
    case 4: return "PUT";
    case 5: return "DELETE";
    case 6: return "TRACE";
    case 7: return "CONNECT";

    default:
        return "INVALID";
        break;
    }
}

std::string constructHeaders(const HeadersMapType &headers) {
    std::string headersString;
    for (const auto &head : headers) {
        headersString.append(head.first);
        headersString.append(": ");
        headersString.append(head.second);
        headersString.append("\r\n");
    }

    return headersString;
}

bool parseStatusLine(const std::string &line, std::string &httpVer, int &statusCode, std::string &reason) {
    auto lineClean = line.substr(0, line.find("\r\n"));

    // do this check just in case the server is drunk or retarded, would be nice to do more checks with some regex
    if (!lineClean.starts_with("HTTP/"))
        return false; // what

    const auto firstSpace = lineClean.find(' ');
    if (firstSpace == std::string::npos)
        return false;

    const std::string fullHttpVerStr = lineClean.substr(0, firstSpace);
    const std::string httpVerStr = fullHttpVerStr.substr(strlen("HTTP/"));

    if (!std::isdigit(httpVerStr[0]))
        return false;

    if (lineClean.length() < fullHttpVerStr.length() + strlen(" XXX"))
        return false; // the rest of the line is less than 3 characters in length, what the fuck happened also abort

    const auto codeAndReason = lineClean.substr(firstSpace + 1);
    const auto statusCodeStr = codeAndReason.substr(0, 3);
    if (!std::isdigit(statusCodeStr[0]) || !std::isdigit(statusCodeStr[1]) || !std::isdigit(statusCodeStr[2]))
        return false; // status code contains non digit characters, abort

    const int statusCodeInt = std::stoi(statusCodeStr);

    std::string reasonStr = "";
    bool hasReason = codeAndReason.find(' ') != std::string::npos;
    if (hasReason) // standard says that reasons CAN be empty, we have to take this edge case into account
        reasonStr = codeAndReason.substr(4);

    httpVer = httpVerStr;
    statusCode = statusCodeInt;
    reason = reasonStr;

    return true;
}

/*
    CANNOT have ANYTHING after the last \r\n or \r\n\r\n else it will be treated as a header
*/
bool parseHeaders(std::string &headersRaw, HeadersMapType &headersOut) {
    char *ptr = strtok(headersRaw.data(), "\r\n");
    // use while loop to check ptr is not null
    while (ptr != NULL) {
        auto line = std::string_view(ptr);

        if (line.find(':') == std::string::npos)
            return false; // separator is missing, the header is invalid

        auto name = line.substr(0, line.find(':'));
        int valueStart = name.length() + 1;
        if (line.find(": ") != std::string_view::npos)
            // Theres a space between semicolon and value, trim it
            valueStart++;

        auto value = line.substr(valueStart);

        headersOut.emplace(std::string(name), std::string(value));
        ptr = strtok(nullptr, "\r\n");
    }
    return true;
}

bool parseResponse(const std::string &res, SceRequestResponse &reqres) {
    auto statusLine = res.substr(0, res.find("\r\n"));
    if (!parseStatusLine(statusLine, reqres.httpVer, reqres.statusCode, reqres.reasonPhrase))
        return false;

    auto headersRaw = res.substr(res.find("\r\n") + strlen("\r\n"), res.find("\r\n\r\n"));

    if (!parseHeaders(headersRaw, reqres.headers))
        return false;

    auto contLenIt = reqres.headers.find("Content-Length");
    if (contLenIt == reqres.headers.end()) {
        reqres.contentLength = 0;
    } else {
        reqres.contentLength = std::stoi(contLenIt->second);
    }

    return true;
}

bool socketSetBlocking(int sockfd, bool blocking) {
#ifdef _WIN32
    u_long blocking_tmp = blocking;
    ioctlsocket(sockfd, FIONBIO, &blocking_tmp);
#else
    if (blocking) { // Blocking
        int flags = fcntl(sockfd, F_GETFL); // Get flags
        fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK); // Set NONBLOCK flag off
    } else { // Non blocking
        int flags = fcntl(sockfd, F_GETFL); // Get flags
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); // Set NONBLOCK flag on
    }
#endif
    return true;
}

std::string get_web_response(const std::string &url) {
    auto curl = curl_easy_init();
    if (!curl)
        return {};

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Vita3K Emulator");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true); // Follow redirects
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

    std::string response_string;
    const auto writeFunc = +[](void *ptr, size_t size, size_t nmemb, std::string *data) {
        data->append((char *)ptr, size * nmemb);
        return size * nmemb;
    };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    long response_code;
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    curl_easy_cleanup(curl);

    if (response_code / 100 == 2)
        return response_string;

    return {};
}

std::string get_web_regex_result(const std::string &url, const std::regex &regex) {
    std::string result;

    // Get the response of the web
    const auto response = get_web_response(url);

    // Check if the response is not empty
    if (!response.empty()) {
        std::smatch match;
        // Check if the response matches the regex
        if (std::regex_search(response, match, regex)) {
            result = match[1];
        } else
            LOG_ERROR("No success found regex: {}", response);
    }

    return result;
}

std::vector<AssignedAddr> get_all_assigned_addrs() {
    std::vector<AssignedAddr> out_addrs;
    const auto ret_addrs = [&out_addrs]() {
        if (out_addrs.empty())
            out_addrs.push_back({ "localhost", "127.0.0.1", "255.255.255.255" });

        return out_addrs;
    };

#ifdef _WIN32
    PIP_ADAPTER_INFO pAdapterInfo;
    DWORD dwRetVal = 0;
    UINT i;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        LOG_CRITICAL("Error allocating memory needed to call GetAdaptersinfo");
        return ret_addrs();
    }
    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            LOG_CRITICAL("Error allocating memory needed to call GetAdaptersinfo");
            return ret_addrs();
        }
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        const std::string noAddress = "0.0.0.0";
        while (pAdapter) {
            IP_ADDR_STRING *pIPAddr = &pAdapter->IpAddressList;
            while (pIPAddr) {
                if (noAddress.compare(pIPAddr->IpAddress.String) != 0)
                    out_addrs.push_back({ pAdapter->Description, pIPAddr->IpAddress.String, pIPAddr->IpMask.String });
                pIPAddr = pIPAddr->Next;
            }
            pAdapter = pAdapter->Next;
        }
    } else {
        LOG_CRITICAL("GetAdaptersInfo failed with error: {}", dwRetVal);
    }
#else
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;
        if ((ifa->ifa_flags & IFF_LOOPBACK) != 0)
            continue;
        if (ifa->ifa_flags)
            if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
                char netMaskAddrStr[INET_ADDRSTRLEN];
                auto netMaskAddr = ((sockaddr_in *)ifa->ifa_netmask)->sin_addr;
                inet_ntop(AF_INET, &netMaskAddr, netMaskAddrStr, INET_ADDRSTRLEN);
                // is a valid IP4 Address
                tmpAddrPtr = &((sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                out_addrs.push_back({ ifa->ifa_name, addressBuffer, netMaskAddrStr });
            }
    }
    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);
#endif
    return ret_addrs();
}

static size_t auto_adapter_index(const std::vector<AssignedAddr> &addrs);

AssignedAddr get_selected_assigned_addr(int32_t &outIndex) {
    const auto addrs = get_all_assigned_addrs();
    // ss-delta: a negative index (adapter = auto) picks the adapter each time, and stays negative
    if (outIndex < 0)
        return addrs[auto_adapter_index(addrs)];
    if (outIndex >= static_cast<int32_t>(addrs.size())) {
        LOG_ERROR("Invalid index {}, returning first address", outIndex);
        outIndex = 0;
    }
    return addrs[outIndex];
}

static int local_adhoc = 0;

// lan mode: this copy's address and broadcast address (init_address), every address of this machine,
// and the peers a broadcast also goes to
static std::mutex lan_mutex;
static uint32_t lan_addr = 0, lan_broadcast = 0;
static std::vector<uint32_t> own_addrs, set_peers, learned_peers;

static std::string ipv4_text(uint32_t addr) {
    in_addr a{};
    a.s_addr = addr;
    char text[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &a, text, sizeof(text));
    return text;
}

static void ensure_winsock() {
#ifdef _WIN32
    // name lookups and the route check may run before the game's sceNetInit (which starts Winsock)
    static const bool started = [] {
        WSADATA data;
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }();
    (void)started;
#endif
}

// the address this machine sends from to reach dest (the routing table's choice; a UDP connect sends nothing)
static uint32_t route_source(uint32_t dest) {
    ensure_winsock();
#ifdef _WIN32
    const SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
        return 0;
#else
    const int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0)
        return 0;
#endif
    sockaddr_in to{};
    to.sin_family = AF_INET;
    to.sin_port = htons(9);
    to.sin_addr.s_addr = dest;
    uint32_t source = 0;
    if (connect(s, reinterpret_cast<const sockaddr *>(&to), sizeof(to)) == 0) {
        sockaddr_in local{};
        socklen_t len = sizeof(local);
        if (getsockname(s, reinterpret_cast<sockaddr *>(&local), &len) == 0)
            source = local.sin_addr.s_addr;
    }
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
    return source;
}

// adapter = auto: the adapter this machine reaches the first peer through, or without peers the one of
// its default route (the internet's; not a virtual adapter that happens to come first); else the first
static size_t auto_adapter_index(const std::vector<AssignedAddr> &addrs) {
    uint32_t dest = 0;
    {
        const std::lock_guard<std::mutex> lock(lan_mutex);
        if (!set_peers.empty())
            dest = set_peers.front();
    }
    if (!dest)
        inet_pton(AF_INET, "8.8.8.8", &dest);
    const uint32_t source = route_source(dest);
    for (size_t i = 0; i < addrs.size(); i++) {
        uint32_t a = 0;
        if (source && inet_pton(AF_INET, addrs[i].addr.c_str(), &a) == 1 && a == source)
            return i;
    }
    return 0;
}

void set_local_adhoc(int player) {
    local_adhoc = (player >= 1 && player <= LOCAL_ADHOC_MAX) ? player : 0;
}

int local_adhoc_player() {
    return local_adhoc;
}

uint32_t local_adhoc_addr(int player) {
    return htonl(0x7F000000u | static_cast<uint32_t>(player));
}

bool is_local_adhoc_broadcast(uint32_t addr) {
    return local_adhoc && (addr == htonl(0x7FFFFFFFu) || addr == 0xFFFFFFFFu);
}

void init_address(int32_t &outIndex, uint32_t &netAddr, uint32_t &broadcastAddr) {
    // the addresses Ad Hoc Play uses, logged once for each change (ss-delta #21: which adapter a player's copy took)
    static std::string logged;
    if (local_adhoc) {
        netAddr = local_adhoc_addr(local_adhoc);
        broadcastAddr = htonl(0x7FFFFFFFu); // 127.255.255.255, sent to each copy (P2PSocket)
        const std::string text = fmt::format("Ad Hoc Play: copies on this PC (network = local), this one 127.0.0.{}", local_adhoc);
        if (text != logged)
            LOG_INFO("{}", logged = text);
        return;
    }
    // Initialize the net and broadcast address based on the assigned address and netmask
    const auto addrs = get_all_assigned_addrs();
    const auto addr = get_selected_assigned_addr(outIndex);
    int netMask;
    inet_pton(AF_INET, addr.addr.c_str(), &netAddr);
    inet_pton(AF_INET, addr.netMask.c_str(), &netMask);
    broadcastAddr = netAddr | ~netMask;
    in_addr broadcast{};
    broadcast.s_addr = broadcastAddr;
    char broadcast_text[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &broadcast, broadcast_text, sizeof(broadcast_text));
    std::string index_text = std::to_string(outIndex);
    if (outIndex < 0)
        for (size_t i = 0; i < addrs.size(); i++)
            if (addrs[i].addr == addr.addr)
                index_text = fmt::format("auto ({})", i);
    std::string text = fmt::format("Ad Hoc Play: network adapter {} of {} ([multiplayer] adapter), {} mask {} ({}), broadcasts to {}",
        index_text, addrs.size(), addr.addr, addr.netMask, addr.name, broadcast_text);
    for (size_t i = 0; i < addrs.size(); i++)
        text += fmt::format("\n  adapter {}: {} mask {} ({})", i, addrs[i].addr, addrs[i].netMask, addrs[i].name);
    {
        const std::lock_guard<std::mutex> lock(lan_mutex);
        lan_addr = netAddr;
        lan_broadcast = broadcastAddr;
        own_addrs.clear();
        for (const auto &a : addrs) {
            uint32_t own = 0;
            if (inet_pton(AF_INET, a.addr.c_str(), &own) == 1)
                own_addrs.push_back(own);
        }
    }
    const auto peers = lan_peers();
    if (!peers.empty()) {
        text += "\n  peers ([multiplayer] peers, a broadcast also goes to each):";
        for (const uint32_t peer : peers)
            text += " " + ipv4_text(peer);
    }
    if (!lan_broadcast_useful())
        text += "\n  (the adapter's broadcast address is its own: only the peers are reached)";
    if (text != logged)
        LOG_INFO("{}", logged = text);
}

void set_lan_peers(const std::vector<uint32_t> &addrs) {
    const std::lock_guard<std::mutex> lock(lan_mutex);
    set_peers = addrs;
}

void set_lan_peers_text(const std::string &list) {
    std::vector<uint32_t> peers;
    std::string resolved;
    for (size_t start = 0; start <= list.size();) {
        size_t end = list.find_first_of(",; ", start);
        if (end == std::string::npos)
            end = list.size();
        const std::string host = list.substr(start, end - start);
        start = end + 1;
        if (host.empty())
            continue;
        const uint32_t addr = resolve_ipv4(host);
        if (!addr) {
            LOG_ERROR("Ad Hoc Play: peer {} doesn't resolve to an IPv4 address", host);
            continue;
        }
        peers.push_back(addr);
        resolved += (resolved.empty() ? "" : ", ") + (host == ipv4_text(addr) ? host : fmt::format("{} ({})", host, ipv4_text(addr)));
    }
    set_lan_peers(peers);
    if (!peers.empty())
        LOG_INFO("Ad Hoc Play: peers {} ([multiplayer] peers)", resolved);
}

void note_lan_peer(uint32_t addr) {
    if (local_adhoc || addr == 0 || addr == 0xFFFFFFFFu || (ntohl(addr) >> 24) == 127)
        return;
    std::string added;
    {
        const std::lock_guard<std::mutex> lock(lan_mutex);
        if (addr == lan_addr || addr == lan_broadcast || std::find(own_addrs.begin(), own_addrs.end(), addr) != own_addrs.end()
            || std::find(set_peers.begin(), set_peers.end(), addr) != set_peers.end()
            || std::find(learned_peers.begin(), learned_peers.end(), addr) != learned_peers.end() || learned_peers.size() >= 16)
            return;
        learned_peers.push_back(addr);
        added = ipv4_text(addr);
    }
    LOG_INFO("Ad Hoc Play: a packet from {}, which now also gets this copy's broadcasts", added);
}

std::vector<uint32_t> lan_peers() {
    const std::lock_guard<std::mutex> lock(lan_mutex);
    std::vector<uint32_t> out = set_peers;
    out.insert(out.end(), learned_peers.begin(), learned_peers.end());
    return out;
}

bool is_lan_broadcast(uint32_t addr) {
    if (local_adhoc)
        return false;
    const std::lock_guard<std::mutex> lock(lan_mutex);
    return addr == 0xFFFFFFFFu || (lan_broadcast != 0 && addr == lan_broadcast);
}

bool lan_broadcast_useful() {
    const std::lock_guard<std::mutex> lock(lan_mutex);
    return lan_broadcast != lan_addr;
}

uint32_t resolve_ipv4(const std::string &host) {
    uint32_t addr = 0;
    if (inet_pton(AF_INET, host.c_str(), &addr) == 1)
        return addr;
    ensure_winsock();
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    addrinfo *res = nullptr;
    if (getaddrinfo(host.c_str(), nullptr, &hints, &res) != 0 || !res)
        return 0;
    addr = reinterpret_cast<const sockaddr_in *>(res->ai_addr)->sin_addr.s_addr;
    freeaddrinfo(res);
    return addr;
}

} // namespace net_utils
