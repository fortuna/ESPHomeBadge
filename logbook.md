# Log Book

In this document I track some thoughts and observations from my development sessions.


## Log

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
