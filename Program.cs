using System;
using System.Collections.Generic;
using System.Linq;

namespace MatchingEngine
{

    class Engine
    {
        private Dictionary<ulong,Book> Books { get; }
        private Dictionary<ulong,Order> Orders { get; }
        public Engine() => (Books, Orders) = (new Dictionary<ulong, Book>(), new Dictionary<ulong, Order>());
        public void AddSymbol(ulong sid) { if (!Books.ContainsKey(sid)) Books[sid] = new Book(); }
        public void RemoveSymbol(ulong sid) { if (Books.ContainsKey(sid)) Books.Remove(sid); }
        public void AddOrder(ulong oid, ulong uid, ulong sid, int price, int size, bool isAsk)
        {
            if(size == 0) return;
            var book = Books[sid];
            var order = new Order(oid,uid,sid,price,size,isAsk);
            if (Match(order,book)) return;
            if(isAsk) book.Asks.Add(order);
            else book.Bids.Add(order);
        }
        public void RemoveOrder(ulong oid)
        {
            if(!Orders.ContainsKey(oid)) return;
            var order = Orders[oid];
            if(order.IsAsk) Books[order.Sid].Asks.Remove(order);
            else Books[order.Sid].Bids.Remove(order);
            Orders.Remove(oid);
        }
        private bool Match(Order order, Book book)
        {
            var isAsk = order.IsAsk;
            var price = order.Price;
            var makerOrders = isAsk ? book.Bids : book.Asks;
            while (order.Size > 0 && makerOrders.Any())
            {
                var makerOrder = makerOrders.First();
                if (isAsk ? makerOrder.Price > price : makerOrder.Price < price) break;
                var tradeSize = Math.Min(order.Size, makerOrder.Size);
                makerOrder.Size -= tradeSize;
                order.Size -= tradeSize;
                if (makerOrder.Size != 0) break;
                Orders.Remove(makerOrder.Oid);
                makerOrders.Remove(makerOrder);
            }
            return order.Size == 0;
        }
        class Order
        {
            public ulong Oid { get; }
            public ulong Uid { get; }
            public ulong Sid { get; }
            public int Price { get; }
            public int Size { get; set; }
            public bool IsAsk { get; }
            public Order(ulong oid, ulong uid, ulong sid, int price, int size, bool isAsk) => 
                (Oid, Uid, Sid, Price, Size, IsAsk) = (oid, uid, sid, price, size, isAsk);
        }
        class Book
        {
            public SortedSet<Order> Asks { get; }
            public SortedSet<Order> Bids { get; }
            public Book() {
                Asks = new SortedSet<Order>(
                    Comparer<Order>.Create((a, b) => 
                        a.Price.Equals(b.Price) ? a.Oid.CompareTo(b.Oid) : a.Price.CompareTo(b.Price)));
                Bids = new SortedSet<Order>(
                    Comparer<Order>.Create((a, b) => 
                        a.Price.Equals(b.Price) ? a.Oid.CompareTo(b.Oid) : (-a.Price).CompareTo(-b.Price)));
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Engine m = new Engine();
            m.AddSymbol(1);
            var watch = System.Diagnostics.Stopwatch.StartNew();
            ulong N = 1000000;
            for (ulong i = 0; i < N; i+=10) {
                m.AddOrder(i+0,2, 1, 8, 5,false);
                m.AddOrder(i+1,2, 1, 5, 5,false);
                m.AddOrder(i+2,1, 1, 8, 4,true);
                m.AddOrder(i+3,1, 1, 12, 1,true);
                m.AddOrder(i+4,3, 1, 9, 5,true);
                m.AddOrder(i+5,4, 1, 8, 4,false);
                m.AddOrder(i+6,3, 1, 7, 7,true);
                m.AddOrder(i+7,4, 1, 6, 2,false);
                m.AddOrder(i+8,5, 1, 8, 8,false);
                m.AddOrder(i+9,5, 1, 13, 2,false);
            }
            watch.Stop();
            Console.WriteLine($"Simulated {N} Trades in {watch.ElapsedMilliseconds}ms");
        }
    }
}
