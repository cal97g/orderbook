//
// Created by Uncle Chu on 12/5/18.
//

#ifndef ORDERBOOK_ORDEREXECUTEDMSG_HPP
#define ORDERBOOK_ORDEREXECUTEDMSG_HPP

#include <ctime>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "LobsterUtil.h"
#include "MessageBase.h"

namespace qi = boost::spirit::qi;
namespace phi = boost::phoenix;

namespace lobster
{

    class OrderExecutedMsg : public OrderMsgBase
    {

    public:
        template <typename Iterator>
        struct order_executed_decoder : decoder_base, qi::grammar<Iterator, OrderExecutedMsg()>
        {
            order_executed_decoder(timespec timestamp, char msgtype);

            qi::rule<Iterator, OrderExecutedMsg()> m_wire_msg; // member variables
        };

    public:

        OrderExecutedMsg() : OrderMsgBase() {}
        OrderExecutedMsg(timespec timestamp, char msgtype, uint64_t orderId, uint32_t shares,
                    uint64_t price, int8_t side, bool visible) : OrderMsgBase(timestamp, msgtype,
                                                                              orderId, shares,
                                                                              price, side),
                                                                 m_visible{visible}
        {}
        bool     m_visible;
    };

    template<typename Iterator>
    OrderExecutedMsg::order_executed_decoder<Iterator>::order_executed_decoder(timespec timestamp, char msgtype) :
            decoder_base(timestamp, msgtype),
            OrderExecutedMsg::order_executed_decoder<Iterator>::base_type(m_wire_msg)
    {
        m_wire_msg = ( qi::ulong_long       >> qi::char_(",")
                          >> qi::int_       >> qi::char_(",")
                          >> qi::ulong_long >> qi::char_(",")
                          >> qi::int_ )
        [qi::_val = phi::construct<OrderExecutedMsg> (
                        m_ts, m_mtype, qi::_1, qi::_3, qi::_5, qi::_7,
                        msgtype == '4' ? true : false )];
    }

} // lobster

#endif //ORDERBOOK_ORDEREXECUTEDMSG_HPP
