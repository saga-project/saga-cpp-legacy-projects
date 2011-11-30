

#ifndef SAGA_PM_MASTER_WORKER_DESCRIPTION_HPP
#define SAGA_PM_MASTER_WORKER_DESCRIPTION_HPP

#include "util.hpp"
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {
    struct worker_description
    {
      saga::url              rm;
      saga::job::description jd;
    };

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_DESCRIPTION_HPP

