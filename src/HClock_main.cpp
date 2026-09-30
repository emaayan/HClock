#if defined(DISPLAY_TFT_NATIVE)
#include "generated/HClockTFT_menu.h" // TFT_eSPI renderer + touch (HClockTFT.emf)
#else
#include "generated/HClock_menu.h"
#endif
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

#if defined(DISPLAY_TFT_NATIVE)
// Touch has no way to scroll a tcMenu list, so on the TFT the country and city
// lists are paged: LIST_PAGE_SIZE entries plus "<< Prev" / "Next >>" rows.
#ifndef LIST_PAGE_SIZE
#define LIST_PAGE_SIZE 5
#endif
struct ListPager
{
    size_t total = 0;
    size_t page = 0;
};
enum PagedRow
{
    PR_ENTRY,
    PR_PREV,
    PR_NEXT
};
static ListPager countryPager, cityPager;

static size_t pageCount(const ListPager &p)
{
    return p.total ? (p.total + LIST_PAGE_SIZE - 1) / LIST_PAGE_SIZE : 1;
}
static size_t pageEntries(const ListPager &p)
{
    const size_t start = p.page * LIST_PAGE_SIZE;
    return p.total > start ? min((size_t)LIST_PAGE_SIZE, p.total - start) : 0;
}
static uint8_t pageRows(const ListPager &p)
{
    return pageEntries(p) + (p.page > 0 ? 1 : 0) + (p.page + 1 < pageCount(p) ? 1 : 0);
}
// Maps a list row to a navigation row or to an entry index in the full list.
static PagedRow pagedRow(const ListPager &p, uint8_t row, size_t &entry)
{
    if (p.page > 0)
    {
        if (row == 0)
            return PR_PREV;
        row--;
    }
    if (row < pageEntries(p))
    {
        entry = p.page * LIST_PAGE_SIZE + row;
        return PR_ENTRY;
    }
    return PR_NEXT;
}
// Handles the Prev/Next rows; returns true when the row was one of them.
static bool pagedNavRow(ListRuntimeMenuItem &item, ListPager &p, PagedRow kind, RenderFnMode mode, char *buffer,
                        int bufferSize, int &result)
{
    if (kind == PR_ENTRY)
        return false;
    switch (mode)
    {
    case RENDERFN_INVOKE:
        p.page = kind == PR_NEXT ? p.page + 1 : p.page - 1;
        item.setNumberOfRows(pageRows(p));
        menuMgr.changeMenu(&item);
        result = true;
        return true;
    case RENDERFN_NAME:
        strncpy(buffer, kind == PR_NEXT ? "Next >>" : "<< Prev", bufferSize);
        result = true;
        return true;
    case RENDERFN_VALUE:
        snprintf(buffer, bufferSize, "%u/%u", (unsigned)(p.page + 1), (unsigned)pageCount(p));
        result = true;
        return true;
    default:
        return false;
    }
}
static void showPageOf(ListRuntimeMenuItem &item, ListPager &p, size_t total, int selectedIndex)
{
    p.total = total;
    p.page = selectedIndex > 0 ? selectedIndex / LIST_PAGE_SIZE : 0;
    item.setNumberOfRows(pageRows(p));
}
#endif

void buildCitiesMenu()
{
    const size_t totalCities = citiesDao.getSize();
#if defined(DISPLAY_TFT_NATIVE)
    int selected = -1;
    if (locations.hasCity())
    {
        const cityCode id = locations.getCurrentCity().id;
        for (size_t i = 0; i < totalCities && selected < 0; i++)
            if (citiesDao.get(i).id == id)
                selected = i;
    }
    showPageOf(menuCity, cityPager, totalCities, selected);
#else
    menuCity.setNumberOfRows(totalCities);
#endif
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
#if defined(DISPLAY_TFT_NATIVE)
    int selected = -1;
    if (locations.hasCountry())
        for (size_t i = 0; i < totalCountries && selected < 0; i++)
            if (strcmp(countriesDAO.get(i).code, locations.getCurrentCountry()) == 0)
                selected = i;
    showPageOf(menuCountry, countryPager, totalCountries, selected);
#else
    menuCountry.setNumberOfRows(totalCountries);
#endif
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
#if defined(DISPLAY_TFT_NATIVE)
        buildCountriesMenu(); // open the country list on the saved country's page
#endif
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

#if defined(DISPLAY_TFT_NATIVE)
        nativeFaceRelease();          // the menu uses TFT_eSPI's own fonts
        tft.fillScreen(TFT_BLACK);    // the menu repaints on a clean screen
        _disp.setContrast(_contrast); // full brightness while in the menu
        _faceBacklight = -1;          // the face re-applies night dimming when it returns
#endif
        renderer.giveBackDisplay();
        inMenu = true;
    }
    else
    {
#if defined(DISPLAY_TFT_NATIVE)
        if (inMenu)
            nativeFaceInvalidate(); // back from the menu: redraw the whole face
#endif
        inMenu = false;
        display();
         // Optional: yield if using task manager to avoid blocking
         //taskManager.yieldForMicros(1000);
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
#if defined(LAYOUT_256X128)
    // Big screen shows everything at once, so long-press is free for brightness.
    increase_button.attachLongPressStart(brightnessUp);
    decrease_button.attachLongPressStart(brightnessDown);
#else
    increase_button.attachLongPressStart(leftScreen);
    increase_button.attachLongPressStop(resetScreen);
    decrease_button.attachLongPressStart(rightScreen);
    decrease_button.attachLongPressStop(resetScreen);
#endif
}


#if defined(DISPLAY_TFT_NATIVE)
void handleFaceTap(int x, int y)
{
    switch (nativeFaceTap(x, y))
    {
    case FACE_MENU:
        menuMgr.onMenuSelect(true); // same as holding OK
        break;
    case FACE_BRIGHTER:
        brightnessUp();
        break;
    case FACE_DIMMER:
        brightnessDown();
        break;
    default:
        break;
    }
}

// Taps on the clock face (tcMenu ignores touches while the face owns the screen).
class FaceTouchObserver : public TouchObserver
{
    uint32_t ignoreUntil = 0;

public:
    void touched(const TouchNotification &n) override
    {
        if (inMenu)
        {
            ignoreUntil = millis() + 600; // the tap that closes the menu must not open the bar
            return;
        }
        if (n.getTouchState() != iotouch::TOUCHED || (int32_t)(millis() - ignoreUntil) < 0)
            return;
        const Coord c = n.getCursorPosition();
        handleFaceTap(c.x, c.y);
    }
} faceTouch;
#endif

#include "SerialConsole.h"

void setup()
{
    consoleBegin();
    
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
#if defined(DISPLAY_TFT_NATIVE)
                tft.setRotation(TFT_ROTATION); // setupMenu() (generated) set rotation 1
                // touch calibration was made at rotation 1: at 3 (turned 180°) both axes are mirrored
                touchScreen.changeOrientation(iotouch::TouchOrientationSettings(false, TFT_ROTATION == 3, TFT_ROTATION == 3));
                nativeTouchInit();
                touchScreen.setSecondaryObserver(&faceTouch);
#endif
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
    consolePoll();
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
    size_t entry = row; // index in the full country list
#if defined(DISPLAY_TFT_NATIVE)
    if (!isTitle)
    {
        int result;
        if (pagedNavRow(menuCountry, countryPager, pagedRow(countryPager, row, entry), mode, buffer, bufferSize, result))
            return result;
    }
#endif
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
            const Country &country = countriesDAO.get(entry);
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
            const Country &country = countriesDAO.get(entry);
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
            const Country &country = countriesDAO.get(entry);
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
    size_t entry = row; // index in the full city list
#if defined(DISPLAY_TFT_NATIVE)
    if (!isTitle)
    {
        int result;
        if (pagedNavRow(menuCity, cityPager, pagedRow(cityPager, row, entry), mode, buffer, bufferSize, result))
            return result;
    }
#endif
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
            const City &ct = citiesDao.get(entry);
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
            const City &ct = citiesDao.get(entry);
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
            fastltoa(buffer, entry, 3, NOT_PADDED, bufferSize);
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
