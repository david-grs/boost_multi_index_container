#include "message_handler.h"

int main()
{
  market_data_provider mdp;
  mdp.add_stock(stock{"1", "1", 10.0, 100});

  return 0;
}
