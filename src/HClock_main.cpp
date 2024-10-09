#include "generated/HClock_menu.h"
#include <HClock.h>

#include <FilesLib.hpp>
#include <Locations.hpp>
#include <LocationsDAO.hpp>
#include <EEPROMSettings.hpp>
#include <LocationsSettings.hpp>
#include <SettingsLib.hpp>
#include <ConfigMenuManagerObserver.hpp>

#include <OneButton.h>
/* #region  Buttons */

#ifndef GREEN_BUTTON
#define GREEN_BUTTON 25
#endif

#ifndef BLUE_BUTTON
#define BLUE_BUTTON 26
#endif

#ifndef PULL_UP
#define PULL_UP false
#endif

#ifndef BRIGHTNESS_PIN
#define BRIGHTNESS_PIN 6
#endif

OneButton increase_button(GREEN_BUTTON, PULL_UP);
OneButton decrease_button(BLUE_BUTTON, PULL_UP);
void onTickButtons()
{

    increase_button.tick();
    decrease_button.tick();
}

bool setCityLocation()
{
    if (locations.hasCountry() && locations.hasCity())
    {
        char *code = locations.getCurrentCountry();
        const Country &country = countriesDAO.findBy(code);
        City &ct = locations.getCurrentCity();
        const location loc = {ct.lat, ct.lng, ct.elevation};
        setLocation(loc, country.isIsrael);
        settz(ct.dstRules);
        return true;
    }
    else
    {
        return false;
    }
}

void buildCitiesMenu()
{
    const size_t totalCities = citiesDao.getSize();
    menuCity.setNumberOfRows(totalCities);
}

bool onCitySelect(const City &ct)
{
    if (!citiesDao.isNull(ct))
    {
        if (locations.setCurrentCity(ct.id))
        {
            locationSettings.saveCity();
        }

        setCityLocation();
        menuMgr.resetMenu(false);
        menuMgr.changeMenu(&menuCity);
    }
    return true;
}

size_t buildCountriesMenu()
{
    status status = loadCountries();
    const size_t totalCountries = countriesDAO.getSize();
    menuCountry.setNumberOfRows(totalCountries);
    return totalCountries;
}

bool onCountrySelect(const Country &country)
{
    if (locations.setCurrentCountry(country.code))
    {
        locationSettings.saveCountry();
        locationSettings.saveCity();
    }
    buildCitiesMenu();
    menuMgr.resetMenu(false);
    menuMgr.changeMenu(&menuCity);
    return true;
}

bool initSystem()
{

    u_int32_t initConf = locationSettings.loadInit();
    size_t totalCountries = buildCountriesMenu();
    if (totalCountries > 0)
    {
        locationSettings.loadCountry();
        locationSettings.loadCity();
        if (!locations.hasCountry())
        {
            const Country &ctr = countriesDAO.get(0);
            locations.setCurrentCountry(ctr.code);
        }
        buildCitiesMenu();
        menuMgr.addChangeNotification(&confObserver);
        //  menuMgr.setItemCommittedHook(&onCommit);
        return true;
    }
    else
    {

        return false;
    }
}

int ret = false;
bool inMenu = false;
void displayCallback(unsigned int encoderValue, RenderPressMode clicked)
{

    if (clicked == RPRESS_HELD)
    {
        const tm tm = getTM();

        const DateStorage ds = DateStorage(tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        menuSetDate.setDate(ds);

        const TimeStorage ts = TimeStorage(tm.tm_hour, tm.tm_min, tm.tm_sec);
        menuSetTime.setTime(ts);

        renderer.giveBackDisplay();
        inMenu = true;
    }
    else
    {
        inMenu = false;
        display();
    }
}
void increaseDate()
{
    if (!inMenu)
    {
        TMWrapper tmw = getNow();
        setNow(tmw.modifyDay(1));
    }
}
void decreaseDate()
{
    if (!inMenu)
    {
        TMWrapper tmw = getNow();
        setNow(tmw.modifyDay(-1));
    }
}
void leftScreen()
{
    setScreenState(1);
}

void rightScreen()
{
    setScreenState(2);
}

 #define QUICK_DEBUG
void setupButtons()
{
#ifdef QUICK_DEBUG
    increase_button.attachClick(increaseDate);
    decrease_button.attachClick(decreaseDate);
#endif
    increase_button.attachLongPressStart(leftScreen);
    increase_button.attachLongPressStop(resetScreen);
    decrease_button.attachLongPressStart(rightScreen);
    decrease_button.attachLongPressStop(resetScreen);
}

void setup()
{
    
    setupButtons();
    bool r = initFS();
    if (r)
    {
        setupMenu();
        ret = initSystem();
        if (ret)
        {
            if (locations.hasCity())
            {
                City &ct = locations.getCurrentCity();
                setCityLocation();

                writeMessage(0, 50, "JClock: %s", ct.name);
                delay(5000);

                init();
                renderer.takeOverDisplay(displayCallback);
            }
            else
            {
                writeMessage("No %s", "City Configured");
            }
        }
        else
        {
            writeMessage("No %s", "Countries"); // TODO: try and see how to init seperately the display
        }
    }
    else
    {
        writeMessage("Failed %s", "File System");
    }
 //   Serial.begin(115200);
//   Serial.println("Starting");
}

void loop()
{
   // Serial.println("Hello");
    if (ret)
    {
        onTickButtons();
        //    display();
        taskManager.runLoop();
    }
}

void CALLBACK_FUNCTION onSetDate(int id)
{
    const tm tm = getTM();
    const DateStorage ds = menuSetDate.getDate();
    const TMWrapper set_tmw = TMWrapper(ds.year - 1900, ds.month - 1, ds.day, tm);
    _rtc.changeTime(set_tmw);
}

void CALLBACK_FUNCTION onSetTime(int id)
{
    const tm tm = getTM();
    const TimeStorage ts = menuSetTime.getTime();
    const TMWrapper set_tmw = TMWrapper(tm, ts.hours, ts.minutes, ts.seconds);
    _rtc.changeTime(set_tmw);
}

void CALLBACK_FUNCTION onExit(int id)
{
    renderer.takeOverDisplay(displayCallback);
}

// This callback needs to be implemented by you, see the below docs:
//  1. List Docs - https://www.thecoderscorner.com/products/arduino-libraries/tc-menu/menu-item-types/list-menu-item/
//  2. ScrollChoice Docs - https://www.thecoderscorner.com/products/arduino-libraries/tc-menu/menu-item-types/scrollchoice-menu-item/
int CALLBACK_FUNCTION fnCountryRtCall(RuntimeMenuItem *item, uint8_t row, RenderFnMode mode, char *buffer, int bufferSize)
{
    const bool isTitle = row == LIST_PARENT_ITEM_POS;
    switch (mode)
    {
    case RENDERFN_INVOKE:
    {
        if (isTitle)
        {
            return false;
        }
        else
        {
            const Country &country = countriesDAO.get(row);
            return onCountrySelect(country);
        }
    }
    case RENDERFN_NAME:
    {
        if (isTitle)
        {
            strncpy(buffer, "Countries", bufferSize);
        }
        else
        {
            const Country &country = countriesDAO.get(row);
            const char *countryName = country.name;
            strncpy(buffer, countryName, bufferSize > 15 ? 15 : bufferSize);
            // fastltoa(buffer, row, 3, NOT_PADDED, bufferSize);
        }
        return true;
    }
    case RENDERFN_VALUE:
    {
        if (isTitle)
        {
            char buf[11] = "";
            if (locations.hasCountry())
            {
                snprintf(buf, sizeof(buf), "[%.*s]", 8, locations.getCurrentCountry());
            }
            else
            {
                snprintf(buf, sizeof(buf), "%s", ">>"); // at this stage didn't load countries yet
            }
            strncpy(buffer, buf, bufferSize);
        }
        else
        {
            const Country &country = countriesDAO.get(row);
            const char *countryCode = country.code;
            strncpy(buffer, countryCode, bufferSize);

            // ltoaClrBuff(buffer, row, 3, NOT_PADDED, bufferSize);
        }
        return true;
    }

    case RENDERFN_EEPROM_POS:
        return 0xffff; // lists are generally not saved to EEPROM
    default:
        return defaultRtListCallback(item, row, mode, buffer, bufferSize);
    }
}

// This callback needs to be implemented by you, see the below docs:
//  1. List Docs - https://www.thecoderscorner.com/products/arduino-libraries/tc-menu/menu-item-types/list-menu-item/
//  2. ScrollChoice Docs - https://www.thecoderscorner.com/products/arduino-libraries/tc-menu/menu-item-types/scrollchoice-menu-item/
int CALLBACK_FUNCTION fnCityRtCall(RuntimeMenuItem *item, uint8_t row, RenderFnMode mode, char *buffer, int bufferSize)
{
    const bool isTitle = row == LIST_PARENT_ITEM_POS;
    switch (mode)
    {
    case RENDERFN_INVOKE:
    {
        if (isTitle)
        {
            return false;
        }
        else
        {
            const City &ct = citiesDao.get(row);
            return onCitySelect(ct);
        }
    }
    case RENDERFN_NAME:
    {
        if (isTitle)
        {
            strncpy(buffer, "Cities", bufferSize);
        }
        else
        {
            const City &ct = citiesDao.get(row);
            strncpy(buffer, ct.name, bufferSize);
        }
        return true;
    }
    case RENDERFN_VALUE:
    {
        if (isTitle)
        {

            char buf[11] = "";
            if (locations.hasCity())
            {
                const City &ct = locations.getCurrentCity();
                snprintf(buf, sizeof(buf), "[%.*s]", 8, ct.name);
            }
            else
            {
                snprintf(buf, sizeof(buf), "(%d)", citiesDao.getSize());
            }
            strncpy(buffer, buf, bufferSize);
        }
        else
        {
            fastltoa(buffer, row, 3, NOT_PADDED, bufferSize);
        }
        return true;
    }
    case RENDERFN_EEPROM_POS:
    {
        return 0xffff; // lists are generally not saved to EEPROM
    }
    default:
        return defaultRtListCallback(item, row, mode, buffer, bufferSize);
    }
}
