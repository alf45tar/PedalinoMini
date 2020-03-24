var virtualButtons = (function() {
  function render(props) {
    return function(tok, i) { return (i % 2) ? props[tok] : tok; };
  }

  function renderTemplate(template, props) {
    let wrapper = document.createElement('template');
    wrapper.innerHTML = template.map(render(props)).join('');
    return wrapper.content.firstChild;
  }

  var pedalTxt = document.querySelector('script[data-template="vbutton_pedal"]').innerHTML.split(/\$\{(.+?)\}/g);
  var actionTxt = document.querySelector('script[data-template="vbutton_pedalaction"]').innerHTML.split(/\$\{(.+?)\}/g);

  function triggerOnChange(elmt) {
    let evt = document.createEvent('HTMLEvents');
    evt.initEvent('change', true, true);
    elmt.dispatchEvent(evt);
  }

  function createActionUI(pedal, button, action, info) {
    let props = {
      myid: "" + pedal + "_" + button + "_" + action
    };
    let ui = renderTemplate(actionTxt, props);
    let midiDiv = ui.querySelector(".mididiv");
    let midiProgramDiv = ui.querySelector(".midiprogramdiv");
    let pedBankDiv = ui.querySelector(".pedbankdiv");
    if (info !== undefined) {
      ui.querySelector("select.type").value = info.type;
      if ([1, 2, 3].includes(info.type)) {
        midiDiv.querySelector("input.channel").value = info.channel;
        midiDiv.querySelector("input.code").value = info.code;
        midiDiv.querySelector("input.value").value = info.value;
      }
      if ([4, 5, 6, 7].includes(info.type)) {
        midiProgramDiv.querySelector("input.channel").value = info.channel;
        midiProgramDiv.querySelector("input.cc").value = info.cc;
        midiProgramDiv.querySelector("input.min").value = info.min;
        midiProgramDiv.querySelector("input.max").value = info.max;
      }
      if ([8, 9, 10].includes(info.type)) {
        pedBankDiv.querySelector("select.min").value = info.min;
        pedBankDiv.querySelector("select.max").value = info.max;
      }
    }
    ui.querySelector("select.type").addEventListener("change", function(event) {
      midiDiv.style.display = ["1", "2", "3"].includes(event.target.value) ? "" : "none";
      midiProgramDiv.style.display = ["4", "5", "6", "7"].includes(event.target.value) ? "" : "none";
      pedBankDiv.style.display = ["8", "9", "10"].includes(event.target.value) ? "" : "none";

      // Only show "CC" input for MIDI CC UP / DOWN
      midiProgramDiv.querySelectorAll(".cc").forEach(function(elm) {
        elm.style.display = ["4", "5"].includes(event.target.value) ? "" : "none";
      });

      // Only show "Max" input for PED BANK UP / DOWN
      pedBankDiv.querySelectorAll(".max").forEach(function(elm) {
        elm.style.display = ["8", "9"].includes(event.target.value) ? "" : "none";
      });
    });
    triggerOnChange(ui.querySelector("select.type"));

    return ui;
  }

  const MAX_ACTIONS = 4;
  const ACTION_LABELS = {
    "0": ["when pressed:", "when released:"],
    "1": ["when pressed (and then toggle to next case):", "when pressed again (and toggle to previous case):"],
    "2": ["when released (and then toggle to next case):", "when released again (and toggle to previous case):"],
    "3": ["after a short press (but not quickly pressed again):",
          "after a long press"
          "after a double-press with short release:",
          "after a double-press with long release:" ]
  };

  return {
    createPedalUI: function(pedal, button, info) {
      let props = {
        p: pedal,
        P: pedal + 1,
        b: button,
        B: button + 1
      };
      let ui = renderTemplate(pedalTxt, props);

      let actionContainer = ui.querySelector("div.actions");
      let actionUis = [];
      for (let action = 0; action < MAX_ACTIONS; action++) {
        let actionUi = createActionUI(pedal, button, action, info.actions[action]);
        actionUis.push(actionUi);
        actionContainer.appendChild(actionUi);
      }

      ui.querySelector("select.mode").value = info.mode;
      ui.querySelector("select.mode").addEventListener("change", function(event) {
        // actionUi 0 and 1 are always visible.
        actionUis[2].style.display = (event.target.value == "3") ? "initial" : "none";
        actionUis[3].style.display = (event.target.value == "3") ? "initial" : "none";

        let labels = ACTION_LABELS[event.target.value];
        for (let action = 0; action < labels.length; action++) {
          actionUis[action].querySelector(".actionlabel").innerHTML = labels[action];
        }
      });
      triggerOnChange(ui.querySelector("select.mode"));

      document.getElementById("mainui").appendChild(ui);
    }
  };
})();
