/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*


*/

#include "ns3/object.h"//of Object class
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h" //for Tra

#include <iostream>

using namespace ns3;

class MyObject : public Object{

public:
  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void){

    static TypeId tid = TypeId ("MyObject")// what this
      .SetParent<Object> ()
      .SetGroupName ("Tutorial")
      .AddConstructor<MyObject> ()//函数的作用是在用户使用对象工厂机制时，不需要考虑类的具体细节就可gou jian
      .AddTraceSource ("MyInteger",
                       "An integer value to trace.",
                       MakeTraceSourceAccessor (&MyObject::m_myInt),// log m_myInt as a trace val
                       "ns3::TracedValueCallback::Int32")
    ;
    return tid;
  }

  MyObject () {}
  TracedValue<int32_t> m_myInt;
};

void
IntTrace (int32_t oldValue, int32_t newValue){
  std::cout << "Traced " << oldValue << " to " << newValue << std::endl;
}

int
main (int argc, char *argv[]){
  Ptr<MyObject> myObject = CreateObject<MyObject> ();
  myObject->TraceConnectWithoutContext ("MyInteger", MakeCallback (&IntTrace));//只有调用了这个函数后，当Trace Sources的数据m_myInt 发生改变时，IntTrace函数才会被调用。
  myObject->m_myInt = 1234;
}
