#include <FrontEndAMB/ambDeviceImpl.h>
#include <FrontEndAMB/ambInterface.h>

AmbDeviceImpl::AmbDeviceImpl(const std::string& name)
  : AmbDeviceInt(),
    name_m(name)
    {}

AmbDeviceImpl::~AmbDeviceImpl() {
}

