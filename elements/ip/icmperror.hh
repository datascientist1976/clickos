#ifndef ICMPERROR_HH
#define ICMPERROR_HH

/*
 * =c
 * ICMPError(src-ip, type, code)
 * =d
 * Generate an ICMP error packet, with specified type and code,
 * in response to an incoming IP packet. The output is an IP/ICMP packet.
 * The ICMP packet's IP source address will the the src-ip
 * configuration argument. The error packet will include (as payload)
 * the original packet's IP header and the first 8 byte of the packet's
 * IP payload. ICMPError sets the packet destination IP and
 * fix_ip_src annotations.
 *
 * The intent is that elements that give rise to errors, like DecIPTTL,
 * should have two outputs, one of which is connected to an ICMPError.
 * Perhaps the ICMPError()s should be followed by a rate limiting
 * element.
 *
 * ICMPError never generates a packet in response to an ICMP error packet,
 * a fragment, or a link broadcast.
 *
 * The output of ICMPError should be connected to the routing lookup
 * machinery, much as if the ICMP errors came from a hardware interface.
 *
 * If the type is 12 and the code is 0 (Parameter Problem), ICMPError
 * takes the error pointer from the packet's param_off annotation.
 * The IPGWOptions element sets the annotation.
 *
 * If the type is 5, produces an ICMP redirect message. The gateway
 * address is taken from the destination annotation. Usually a
 * Paint/CheckPaint element pair hands the packet to a redirect ICMPError.
 * RFC1812 says only code 1 (host redirect) should be used.
 *
 * =e
 * This configuration fragment produces ICMP Time Exceeded error
 * messages in response to TTL expirations, but limits the
 * rate at which such messages can be sent to 10 per second:
 *
 * dt : DecIPTTL(); <br>
 * dt[1] -> ICMPError(18.26.4.24, 11, 0) -> Shaper(1, 10) -> ...
 * =n
 * ICMPError can't decide if the src or dst is an IP directed broadcast
 * address; it is supposed to ignore packets with such addresses.
 *
 * =a DecIPTTL
 * =a FixIPSrc
 * =a IPGWOptions
 */

#include "element.hh"

class ICMPError : public Element {
public:
  ICMPError();
  ~ICMPError();
  
  const char *class_name() const		{ return "ICMPError"; }
  Processing default_processing() const	{ return AGNOSTIC; }
  ICMPError *clone() const { return(new ICMPError); }
  int configure(const String &, ErrorHandler *);
  int initialize(ErrorHandler *errh);

  Packet *simple_action(Packet *);
  
private:

  IPAddress _src_ip;
  int _type;
  int _code;

  bool is_error_type(int);
  bool unicast(struct in_addr);
  bool valid_source(struct in_addr);
  bool has_route_opt(struct ip *ip);
};

#endif
