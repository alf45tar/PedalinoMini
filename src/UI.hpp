#pragma once

#include <StreamString.h>

extern String page;

namespace UI {

template <typename T>
void select(String name, String cssClass, T &&mkbody) {
  page += F("<select class='custom-select custom-select-sm ");
  page += cssClass;
  page += F("' name='");
  page += name;
  page += F("' id='");
  page += name;
  page += F("'>");
  mkbody();
  page += F("</select>");
}

template <typename T>
void select(String name, T &&mkbody) {
  select(name, "", mkbody);
}

void option(String title, String value, bool selected) {
  page += F("<option value='");
  page += value;
  page += F("'");
  if (selected) {
    page += F(" selected");
  }
  page += F(">");
  page += title;
  page += F("</option>");
}

void bankInput(String name, uint8_t banks, uint8_t selected, String cssClass) {
  select(name, cssClass, [&] {
    for (uint8_t bank = 0; bank < banks; bank++) {
      option(String("Bank ") + (bank + 1), String(bank), bank == selected);
    }
  });
}

  void label(String body) {
    page += F("<label class='font-weight-bold'>");
    page += body;
    page += F("</label>");
  }

  void numberInput(String name, int32_t min, int32_t max, int32_t value, String cssClass = "") {
    page += F("<input type='number' class='form-control form-control-sm ");
    page += cssClass;
    page += F("' name='");
    page += name;
    page += F("' min='");
    page += min;
    page += F("' max='");
    page += max;
    page += F("' value='");
    page += value;
    page += F("'>");
  }

  template <typename T>
  void form_row(T &&mkbody) {
    page += F("<div class='form-row'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void form_row(String id, T &&mkbody) {
    page += F("<div class='form-row' id='");
    page += id;
    page += F("'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void form_row(String id, String cssClass, T &&mkbody) {
    page += F("<div class='form-row ");
    page += cssClass;
    page += F("' id='");
    page += id;
    page += F("'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_(uint8_t n, T &&mkbody) {
    page += F("<div class='col-");
    page += n;
    page += "'>";
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_(uint8_t n, String id, String cssClass, T &&mkbody) {
    page += F("<div id='");
    page += id;
    page += F("' class='col-");
    page += n;
    page += F(" ");
    page += cssClass;
    page += F("'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_1(T &&mkbody) {
    page += F("<div class='col-1'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_2(T &&mkbody) {
    page += F("<div class='col-2'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_2r(T &&mkbody) {
    page += F("<div class='col-2 text-right'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_3(T &&mkbody) {
    page += F("<div class='col-3'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_3r(T &&mkbody) {
    page += F("<div class='col-3 text-right'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_4(T &&mkbody) {
    page += F("<div class='col-4'>");
    mkbody();
    page += F("</div>");
  }

  template <typename T>
  void col_6(T &&mkbody) {
    page += F("<div class='col-4'>");
    mkbody();
    page += F("</div>");
  }

  void col_2body(String body, String id="") {
    page += F("<div class='col-2' id='");
    page += id;
    page += F("'>");
    page += body;
    page += F("</div>");
  }

  void col_4body(String body, String id="") {
    page += F("<div class='col-4' id='");
    page += id;
    page += F("'>");
    page += body;
    page += F("</div>");
  }

  void col_9body(String body, String id="") {
    page += F("<div class='col-9' id='");
    page += id;
    page += F("'>");
    page += body;
    page += F("</div>");
  }

  void col_12body(String body, String id="") {
    page += F("<div class='col-12' id='");
    page += id;
    page += F("'>");
    page += body;
    page += F("</div>");
  }

  template <typename T>
  void col_auto(T &&mkbody) {
    page += F("<div class='col-auto'>");
    mkbody();
    page += F("</div>");
  }

  void col_1body(String contents) {
    page += F("<div class='col-1'>");
    page += contents;
    page += F("</div>");
  }

  void span(String contents) {
    page += F("<span>");
    page += contents;
    page += F("</span>");
  }

  void italic(String contents, String id="", String cssClass = "") {
    page += F("<span class='font-italic ");
    page += cssClass;
    page += F("' id='");
    page += id;
    page += F("'>");
    page += contents;
    page += F("</span>");
  }

  void badge(String contents) {
    page += F("<span class='badge badge-primary'>");
    page += contents;
    page += F("</span>");
  }

  void submitButton(String body) {
    page += F("<button type='submit' class='btn btn-primary'>");
    page += body;
    page += F("</button>");
  }

  void submitButton() {
    submitButton(F("Save"));
  }

  void script(String js) {
    page += F("<script language='javascript'>");
    page += js;
    page += F("</script>");
  }

  // Good tip from here
  // https://stackoverflow.com/questions/18673860/defining-a-html-template-to-append-using-jquery/46699845
  template <typename T>
  void scriptTemplate(String name, T &&mkbody) {
    page += F("<script type='text/template' data-template='");
    page += name;
    page += F("'>");
    mkbody();
    page += F("</script>");
  }
}
