//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSAUCTIONUPDATEMSG_HPP
#define PITCH_SPIRIT_BATSAUCTIONUPDATEMSG_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "BATSMessageBase.h"
#include "BATSUtil.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

class BATSAuctionUpdateMsg : public BATSMessageBase
{

public:
    // nested class for decoding the wire msg
    template<typename Iterator>
    struct auction_update_decoder : qi::grammar<Iterator, BATSAuctionUpdateMsg()>
    {
        auction_update_decoder(char msgtype);

        qi::rule<Iterator, BATSAuctionUpdateMsg()> m_wire_msg; // member variables
        qi::rule<Iterator, uint64_t> m_price;
        qi::rule<Iterator, fixed_point() > m_fixed_point;
    };

public:
    BATSAuctionUpdateMsg() : BATSMessageBase() {}
    BATSAuctionUpdateMsg(int timestamp, char msgtype, std::string const& symbol, char auction_type,
                         uint64_t reference_price, uint32_t buyshares, uint32_t sellshares, uint64_t indicative_price,
                         uint64_t auction_only_price) :
            BATSMessageBase(timestamp, msgtype),
            m_symbol(symbol),
            m_auction_type(auction_type),
            m_reference_price(reference_price),
            m_buyshares(buyshares),
            m_sellshares(sellshares),
            m_indicative_price(indicative_price),
            m_auction_only_price(auction_only_price)
    {
    }
    
    std::string m_symbol;
    char m_auction_type;
    uint64_t m_reference_price;
    uint32_t m_buyshares;
    uint32_t m_sellshares;
    uint64_t m_indicative_price;
    uint64_t m_auction_only_price;

    static constexpr auto auctionTypes{"OCHI"};
};

template<typename Iterator>
BATSAuctionUpdateMsg::auction_update_decoder<Iterator>::auction_update_decoder(char msgtype) :
        BATSAuctionUpdateMsg::auction_update_decoder<Iterator>::base_type(m_wire_msg)
{
    // order and execution ids are 12 characters base 36
    qi::uint_parser< uint32_t, 10, 10, 10 > p_shares;
    qi::uint_parser< uint32_t, 10, 10, 10 > m_price;
    qi::uint_parser< uint32_t, 10,  8,  8 > p_ts;

    m_wire_msg    = ( p_ts >> qi::char_(msgtype)
                           >> qi::as_string[ qi::repeat(8)[qi::char_] ]
                           >> qi::char_(BATSAuctionUpdateMsg::auctionTypes)
                           >> m_price
                           >> p_shares
                           >> p_shares
                           >> m_price
                           >> m_price )
        [qi::_val = phi::construct<BATSAuctionUpdateMsg>(
                qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6, qi::_7, qi::_8, qi::_9)];

}


#endif //PITCH_SPIRIT_BATSAUCTIONUPDATEMSG_HPP
