

use messages::AuctionSummaryMsg;
use messages::AuctionUpdateMsg;
use messages::AddOrderMsg;
use messages::OrderCancelMsg;
use messages::OrderExecutedMsg;
use messages::RetailPriceImproveMsg;
use messages::TradeBreakMsg;
use messages::TradeMsg;
use messages::TradingStatusMsg;
use messages::BATSMsgFactory;

use orderbook::PriceBucket;
use orderbook::Order;
use orderbook::OrderManager;
use orderbook::AskBook;
use orderbook::BestPrice;
use orderbook::LimitOrderBook;

use std::env;
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use std::{thread, time};

#[test]
fn test_parse_auction_summary() {
    let msg = "28800168JAAPLSPOTC00010068000000020000";
    let res = AuctionSummaryMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());
}

#[test]
fn test_parse_add_order() {
    let msg = "28800168A1K27GA00000YS000100AAPL  0001831900Y";
    let msg_long = "28800169d1K27GA00000YS000100AAPL  0001831900YBAML";

    let res = AddOrderMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());

    let o = res.unwrap();        
    assert_eq!(o.timestamp, 28800168);
    assert_eq!( o.msg_type, 'A');
    assert_eq!( o.order_id,  204969015920664610);
    assert_eq!( o.side,     'S');
    assert_eq!( o.shares,   100);
    assert_eq!( o.symbol,   "AAPL  ");
    assert_eq!( o.price,    1831900);
    assert_eq!( o.display,  'Y');

    let res = AddOrderMsg::parse_msg(msg_long);
    println!("{:?}", res);
    assert!(res.is_ok());
}

#[test]
fn test_parse_auction_update() {
    let msg = "28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800"; 
    let res = AuctionUpdateMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_order_cancel() {
    let msg = "28800168X1K27GA00000Y000500"; 
    let res = OrderCancelMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_order_executed() {
    let msg = "28800168E1K27GA00000Y0001001K27GA00000K"; 
    let res = OrderExecutedMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_retail_price_improve() {
    let msg = "28800168RAAPLSPOTS"; 
    let res = RetailPriceImproveMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_trade_break() {
    let msg = "28800168B1K27GA00000Y"; 
    let res = TradeBreakMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_trade() {
    let msg = "28800168P1K27GA00000YB000300AAPL  00018319001K27GA00000Z"; 
    let res = TradeMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    

    let msg = "28800168r1K27GA00000YB000300AAPLSPOT00018319001K27GA00000Z"; 
    let res = TradeMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_trade_status() {
    let msg = "28800168HAAPLSPOTT0XY"; 
    let res = TradingStatusMsg::parse_msg(msg);
    println!("{:?}", res);
    assert!(res.is_ok());    
}

#[test]
fn test_parse_file() {
    let path = env::current_dir().unwrap();
    
    println!("The current directory is {}", path.display());
    let filename = "src/pitch_example_data";
    let f = File::open(filename).expect("file not found");
    let f = BufReader::new(f);

    for line in f.lines() {
        let msg = line.unwrap();
        let buf = msg.as_str();
        if buf.chars().nth(8) == Some('J') {
            let res = AuctionSummaryMsg::parse_msg(buf);
            println!("{:?}", res);
            assert!(res.is_ok());
            break;
        }
    }
}

#[test]
fn test_factory() {
    let obj = BATSMsgFactory::parse("28800168A1K27GA00000YS000100AAPL  0001831900Y");
    println!("Return result from msg factory {:?}", obj);
    let msg_obj : Option<AddOrderMsg> = obj.into();
    assert!(msg_obj.is_some());
    println!("After into {:?}", msg_obj);

    let obj = BATSMsgFactory::parse("28800168JAAPLSPOTC00010068000000020000");
    println!("Return result from msg factory {:?}", obj);
    let msg_obj : Option<AuctionSummaryMsg> = obj.into();
    println!("After into {:?}", msg_obj);
    assert!(msg_obj.is_some());
}

#[test]
fn test_price_bucket() {

    let mut pb = PriceBucket::from_price(1200000);
    let o = Order{order_id : 2001, price : 1200000, volume : 100, side : 1, 
                  part_id : String::from("Acme Corp.") };
    let o2 = Order{order_id : 2002, price : 1200000, volume : 220, side : 1, 
                  part_id : String::from("TH Inc.") };     

    pb.add_order( o );
    pb.add_order( o2.clone() );

    assert_eq!( pb.price_level, 1200000 );
    assert_eq!( pb.volume(), 320 );

    pb.remove_order( o2 );

    assert_eq!( pb.volume(), 100 );
}

#[test]
fn test_book() {
    
    let mut book = AskBook::new();
    let o = Order{order_id : 2001, price : 1200000, volume : 150, side : -1, 
                  part_id : String::from("Acme Corp.") };

    let o2 = Order{order_id : 2002, price : 1300000, volume : 220, side : -1, 
                  part_id : String::from("TH Inc.") };     

    book.add_order(o.clone());
    book.add_order(o2.clone());
    assert_eq!(book.volume_at_price_level(1200000), 150);
    assert_eq!(book.volume_at_price_level(1300000), 220);
    book.remove_order(o);
    assert_eq!(book.volume_at_price_level(1200000), 0);
    assert_eq!(book.best_price(), 1200000 );
}

#[test]
fn test_limit_order_book() {

    let mut b = LimitOrderBook::new();
    let o1 = Order{order_id : 2001, price : 10000, volume : 100, side : 1, part_id : String::from("Acme Corp.")};
    let o2 = Order{order_id : 2002, price : 10050, volume : 200, side : 1, part_id : String::from("Acme Corp.")};
    let o3 = Order{order_id : 2003, price : 10100, volume : 300, side : 1, part_id : String::from("Acme Corp.")};
    let o4 = Order{order_id : 2004, price : 10200, volume : 400, side : -1, part_id : String::from("Acme Corp.")};
    let o5 = Order{order_id : 2005, price : 10250, volume : 500, side : -1, part_id : String::from("Acme Corp.")};
    let o6 = Order{order_id : 2006, price : 10300, volume : 600, side : -1, part_id : String::from("Acme Corp.")};

    b.add_order(o1);
    b.add_order(o2);
    b.add_order(o3);
    b.add_order(o4);
    b.add_order(o5);
    b.add_order(o6);

    println!("==========> HERE");
    assert_eq!(b.best_bid(), 10100 );
    assert_eq!(b.best_ask(), 10200 );

    assert_eq!(b.ask_iter().next().unwrap().0, &10200 );
    assert_eq!(b.ask_iter().next_back().unwrap().0, &10300 );

    let o7 = Order{order_id : 2007, price : 10225, volume : 300, side : 1, part_id : String::from("Acme Corp.")};
    b.add_order(o7);
    assert_eq!(b.ask_volume_at_price_level(10200), 100);
}

#[test]
fn test_lob_threading() {

    let mut b = LimitOrderBook::new();
    let o1 = Order{order_id : 2001, price : 10000, volume : 100, side : 1, part_id : String::from("Acme Corp.")};
    println!("----------> testing...", );
    b.start_workers();
    b.add_request(o1);
    //b.wait_till_done();

    thread::sleep(time::Duration::from_secs(5));
}



#[test]
fn test_example() {


}