#ifndef SPONGE_LIBSPONGE_ADDRESS_HH
#define SPONGE_LIBSPONGE_ADDRESS_HH

#include <cstddef>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <utility>

//! Wrapper around [IPv4 addresses](@ref man7::ip) and DNS operations.
class Address {
  public:
    //! \brief Wrapper around [sockaddr_storage](@ref man7::socket).
    //! \details A `sockaddr_storage` is enough space to store any socket address (IPv4 or IPv6).
    class Raw {
      public:
        sockaddr_storage storage_{};  //!< The wrapped struct itself.
         explicit operator sockaddr *();
         explicit operator const sockaddr *() const;
    };

  private:
    socklen_t size_;  //!< Size of the wrapped address.
    Raw address_{};   //!< A wrapped [sockaddr_storage](@ref man7::socket) containing the address.

    //! Constructor from ip/host, service/port, and hints to the resolver.
    Address(const std::string &node, const std::string &service, const addrinfo &hints);

  public:
    //! Construct by resolving a hostname and servicename.
    Address(const std::string &hostname, const std::string &service);

    //! Construct from dotted-quad string ("18.243.0.1") and numeric port.
    explicit Address(const std::string &ip, std::uint16_t port = 0);

    //! Construct from a [sockaddr *](@ref man7::socket).
    Address(const sockaddr *addr,  std::size_t size);

    //! Equality comparison.
    auto operator==(const Address &other) const -> bool;
    auto operator!=(const Address &other) const -> bool { return not operator==(other); }

    //! \name Conversions
    //!@{

    //! Dotted-quad IP address string ("18.243.0.1") and numeric port.
    auto IpPort() const -> std::pair<std::string, uint16_t>;
    //! Dotted-quad IP address string ("18.243.0.1").
    auto Ip() const -> std::string { return IpPort().first; }
    //! Numeric port (host byte order).
    auto Port() const -> uint16_t { return IpPort().second; }
    //! Numeric IP address as an integer (i.e., in [host byte order](\ref man3::byteorder)).
    auto Ipv4Numeric() const -> uint32_t;
    //! Create an Address from a 32-bit raw numeric IP address
    static auto FromIpv4Numeric( uint32_t ip_address) -> Address;
    //! Human-readable string, e.g., "8.8.8.8:53".
    auto ToString() const -> std::string;
    //!@}

    //! \name Low-level operations
    //!@{

    //! Size of the underlying address storage.
    auto Size() const -> socklen_t { return size_; }
    //! Const pointer to the underlying socket address storage.
    explicit operator const sockaddr *() const { return static_cast<const sockaddr*>(address_); }
    //!@}
};

//! \class Address
//! For example, you can do DNS lookups:
//!
//! \include address_example_1.cc
//!
//! or you can specify an IP address and port number:
//!
//! \include address_example_2.cc
//!
//! Once you have an address, you can convert it to other useful representations, e.g.,
//!
//! \include address_example_3.cc

#endif  // SPONGE_LIBSPONGE_ADDRESS_HH
