# Log Book

In this document I track some thoughts and observations from my development sessions.

## To Explore

- Add button combination for factory reset.
- Fix IR Blast focus issue.
- Add WiFi page
- Better Wifi provisioning and web server access. Perhaps simply a factory reset.

## Log

### June 5, 2025 - Badge not portable

I failed to properly demo the badge today, because I couldn't connect to the web server:

- Badge was configured for my wifi, but moved to another place.
- The AP mode triggered, but it would send me to the captive portal, not the web server!
- I configured the wifi to the new network
- I could no longer access the badge. Theories:
  - There were separate 2.4GHz and 5GHz networks with the same name
  - The network prevents communication between devices
- Once configure for the new network, I could no longer trigger the access point or reconfigure the wifi.

TODO:
- Next time I should connect to my phone's AP instead, since I can easily disable it.
- I should have a way to "factory reset" the wifi.
  - Options: UI or holding some button combination.
- Remove the captive portal. Alternatives for provisioning:
  - With a Chrome desktop browser:
    - Improv serial (needs a cable)
    - Improv bluetooth (needs a way to start pairing)
  - Text boxes in the web server. But you still get stuck if you lose access to the web server in the new network.
    We need a way to clear the wifi credentials or force AP mode no matter what.

I should always be able to enable wifi in access point mode and use the web server (the captive portal should not trigger).
However, the captive portal always trigger if the wifi is missing. So we cannot add the captive portal.

Idea:
- Button or combination pressed on boot:
  - disable station mode
    - We can clear the hard-coded settings, but it always tries to load from saved preferences.
  - enable ap mode
  - disable captive portal
- Server can have an option to clear wifi credentials for recovery.

Important points:
- On setup, the wifi component tries to load the saved wifi. Clearing the stations (with `wifi::global_wifi_component::clear_sta()`) doesn't help.
- We can try to save a bogus wifi, but that overwrites the saved preference.

To prevent the captive portal (if present), we can use:

```c++
captive_portal::global_captive_portal = nullptr;
```

And we probably need to call this explicitly:

```c++
WiFiComponent::setup_ap_config_()
```

User stories:
- I want to provision my badge with wifi
  - Connect to AP. Open Web Server. Enter SSID and Password.
- I want to access the badge server to play with the settings and features
- I want to recover from a bad wifi
  - Call wifi.configure with an empty config.

### May 29, 2025 - Pluggable Apps

Let's make it easier to plug apps, so others can contribute.

Questions:
- Will the insertion work as intended?
- Can we have an app boilerplate instead of app_entry boilerplate?
- Can substitutions take objects instead of basic types? Probably not, because there's no way to refer to them.
- Can we remove the need for one group per app? Can leverage hide/unhide. Still needs a separate one for the app selector.
- Can we somehow enable pure C++ apps?
  - Perhaps by leveraging [LvglComponent::add_page](https://api-docs.esphome.io/lvgl__esphome_8cpp_source).
  - Would still need to use YAML for the glyph. But perhaps we can use an image. Could provide an `apps.add_app(lvgl, icon, name, widget)` api.

An app needs:
- Entry in the app switcher
  - Name (maybe also for a title bar)
  - Icon, as a glyph in the Material design font. Font should be shared.
- A widget, to be used as the app root UI element. Probably inserted as pages.
- Behavior, which may be pluggable in various parts.

How to encapsulate apps? We want all of that in a single file, but they get inserted in different places.
We can use [ESPHome Packages]. This makes it easy to include behavior by injecting in the write config
subtree, including the `lvgl.pages` item. Though we need to know the `lvgl` id.

For the menu entry, we can insert into `lvgl[lvgl_id].top_layer.widgets[selector_id]`, but we need boilerplate.
The item boilerplate should be in its own file, and we can `!include` with variables for icon and title.

A sample app file could be named `app_ir_blast.yaml` and look like:

```yaml
font:
  - !extend app_switcher_font
    glyphs:
      - "\U0000ea67"

lgvl:
  - id: !extend badge_lvgl
    top_layer:
      widgets:  # Will this replace the existing widgets field?
        - id: !extend app_selector
          widgets:
            - !include {
              file: app_entry.yaml,
              vars: {icon: "\U0000ea67", title: "IR Blast"}
            }

    pages:
      - id: page_ir_blast
        widgets:
          - button:
              on_press:
                - scripts.execute: send_ir_blast

scripts:
  - id: send_ir_blast
    then:
      # Send code.
```

The app file could look like:

```yaml
packages:
  - !include
      file: app_template.yaml
      vars: {icon: "\U0000ea67", title: "IR Blast"}

lgvl:
  - id: !extend badge_lvgl
    pages:
      - id: page_ir_blast
        widgets:
          - button:
              on_press:
                - scripts.execute: send_ir_blast

scripts:
  - id: send_ir_blast
    then:
      # Send code.
```

We also need boilerplate to create and save an LVGL group for each app.
Perhaps we can eliminate that by using hide/unhide and having one group for all the apps.

I was able to move the apps to their own files, including the text components, which was nice. But had an
issue with the air app, because it everrides a complex sensor that is not available for the Host platform,
and I can't mock it with a template sensor:

```yaml
sensor:
  - id: !extend air_sensor
    eco2:
      on_value:
        - lvgl.label.update:
            id: co2_label
            text: {format: '%.0f', args: [ 'x' ]}
    tvoc:
      on_value:
        - lvgl.label.update:
            id: tvoc_label
            text: {format: '%.0f', args: [ 'x' ]}
```

I also cannot remove it from the [lvgl_host.yaml](./lvgl/lvgl_host.yaml) file because it doesn't
see the sensor.

The problem is that the app is depending on the implementation of the air sensor. That cannot happen.
We can decouple by defining an interface. Let's define scripts `badge_on_eco2_value` and `badge_on_tvoc_value`
that get called by the sensor. Then we can simply insert into those scripts. Those scripts will be part
of the badge API.

[ESPHome Package]: https://esphome.io/components/packages.html


### May 27, 2025 - Clean up selector

Need a way to track the current page to select on the page selector when shown. Decided to go with a [Template Select], which allows for tracking state and provides events. It also allows us to easily change pages from the web server for development or automation.

Ran into an issue: `lvgl.widget.focus` and `lvgl.page.show` are not templatable. We need to use C++ instead. Can use `id(my_select).active_index()` to get the selected index, and use that to get the i-th children with `lv_obj_get_child(parent, i)`, and focus with `lv_group_focus_obj(obj)`.

At this point we are jumping through hoops to work around the ESPHome LVGL restrictions. Should we do the UI in
C++ instead?

The other pain point is adding all the entries to the selector. There's a lot of boilerplate for each entry.
We need to consider programatically populating it, or some sort of template. We may use the [ESPHome Packages]
feature for that.

Flow:
  1. On press:
    - Show the menu, with the current page selected. We need to go from selected page index to the widget to
      focus.
    - Prev/Next moves the selection. This requires going to the next widget to focus. The focus group can
      take care of that.
  1. On release: show page for selection. We need to go from focused item to page to show.

We need to track shown page and focused selection separately.

How do we show a page programatically? We can use [LvglComponent API]: `LvglComponent::show_page(...)`,
`LvglComponent::get_current_page()`, `LvPageType::index`. In that case we don't need the Select object.
We can get the focused object with `lv_group_get_focused(...)` or `lv_group_get_obj_by_index(...)`.

It turns out there are many more widgets in a group than I realized, so the mapping to pages is not working well. I believe that's because all the widgets get added, even if they are not focusable.

I filed a [feature request](https://github.com/esphome/feature-requests/issues/3166) to ESPHome to support declaring groups and setting page groups.

Perhaps I can implement something similar:

1. For each page:
  1. Create page group
    1. For each widget, recursively:
      1. Assign to the page group.

I could implement a `recursive_set_group(obj, group)`.

Can set the page group with `on_boot`. To retrieve, just get the page root group.

If needed, it's possible to use [`lv_obj_set_user_data`] to set special atributes (like the menu index).

Can use the `on_load` and `on_unload` events of a page to update the keypad group.

This is promising: if we hide a widget or any of its ancestors, [it doesn't get focus][focus_next_core]. So we could hide the root of each page to avoid focus crossing the visible page. We still have the issue with the overlay though, because the layer
underneath is not hidden.

To try:
- Two groups: default and selector, switching between them.
- Hide pages that are not being shown with `on_unload`. Show them with `on_load`.

[Template Select]: https://esphome.io/components/select/template
[ESPHome Packages]: https://esphome.io/components/packages.html
[LvglComponent API]: https://api-docs.esphome.io/lvgl__esphome_8cpp_source
[`lv_obj_set_user_data`]: https://docs.lvgl.io/8.0/widgets/obj.html#_CPPv420lv_obj_set_user_dataP8lv_obj_tPv
[focus_next_core]: https://github.com/lvgl/lvgl/blob/e8b95b3f9c91a8b186d5705cb7705c72e6f5aea7/src/core/lv_group.c#L461

### May 25, 2025 - Page Selector

Open questions:
- Does focus stay on one page, or goes across pages? It switched across pages.

I decided to use pages for navigation. My expectation is that pages will fix the cross-screen focus issue.

It turns out focus will switch across pages, so it doesn't fix the problem.

ESPHome lets you nest styles (it compiles), but the nested style seems to be ignored. Perhaps we should use C++ instead.

I moved the LVGL code to its own `lvgl.yaml` file that I can now include into both the badge firmware and the host prototyping code.

TODO:
- Fix focus. Consider not using the system focus for the page selector.

### May 25, 2025 - App Switcher

Open questions:
- How to make the app switcher zoom out and scroll properly?
- Why does everything disappear when the transformed widget has a transparent background?

Working on making a cool App Switcher work.

I want to use the Button 1 (Back/ESC) to make the screen zoom out into a strip that shows all
available screens and you can navigate to switch between them.

I want to use native scrolling to minizime code I need to write and maintain. It also help others
extend.

I found that styles can have transition effects, so I will probably not need to implement my own ([example][Transition Example]).

A scroll callback may be helpful, as used in the [translate on scroll example].

I'm using size transformation to scale the screens, but that breaks positioning and scroll. I found out I need to specify an anchor point for the transformation, but that didn't fix all the issues. After a lot of time debugging, I finally found out that the widget layout size is not affected by the transformation (see [Positions, sizes, and layouts]).

Plan is to create empty containers as proxies for the scaled screens, then anchor the scaled screens to those containers.

[Transition Example]: https://docs.lvgl.io/8.2/overview/style.html#transition
[translate on scroll example]: https://docs.lvgl.io/8/overview/scroll.html#translate-on-scroll
[Positions, sizes, and layouts]: https://docs.lvgl.io/9.2/overview/coord.html#transformation


### Previously - Focus issues

The tabbed navigation did not work well, because the focus moves across tabs, while I need it to stay within a tab. I also didn't like how it looks.

I tried switching to the tileview. It had the same issue. But it gave me the idea of an app switcher where
you can zoom in and out of.
