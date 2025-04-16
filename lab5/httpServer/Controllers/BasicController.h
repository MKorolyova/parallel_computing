#include <iostream>
#include "../DTO/Response.h"
#include "../DTO/Request.h"
#ifndef BASICCONTROLLER_H
#define BASICCONTROLLER_H

class BasicController {
  public:
    virtual void callMethod(Request& request, Response& response) = 0;
};
#endif //BASICCONTROLLER_H
