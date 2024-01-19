#ifndef C680DF15_EDA3_469D_90D7_6BCDDAC1F789
#define C680DF15_EDA3_469D_90D7_6BCDDAC1F789
#ifndef CC3B67AC_6907_4C7E_87EE_1B8D10B5D661
#define CC3B67AC_6907_4C7E_87EE_1B8D10B5D661

#include "generated\HClock_menu.h"

class ConfigMenuManagerObserver : public MenuManagerObserver
{
public:
    void structureHasChanged() override
    {
    }

    bool menuEditStarting(MenuItem *item) override
    {
        const menuid_t menuId = item->getId();
        if (menuBackLocation.getId() == menuId)
        {
        }
        if (menuCountry.getId() == menuId)
        {
            // buildCountriesMenu();
        }

        if (menuCity.getId() == menuId)
        {
        }
        return true; // allow editing to start
    }

    void menuEditEnded(MenuItem *item) override
    {
        const menuid_t menuId = item->getId();
        if (menuCountry.getId() == menuId)
        {
        }
        if (menuCity.getId() == menuId)
        {
        }
    }

    void activeItemHasChanged(MenuItem *newActive) override
    {
    }

} confObserver;

#endif /* CC3B67AC_6907_4C7E_87EE_1B8D10B5D661 */


#endif /* C680DF15_EDA3_469D_90D7_6BCDDAC1F789 */
