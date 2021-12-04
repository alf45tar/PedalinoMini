class t extends HTMLElement {
    static preload()
    {
        import("./start-flash-9b1cffdc.js")
    }
    connectedCallback()
    {
        if (this.renderRoot)
            return;
        if (this.renderRoot = this.attachShadow({
            mode: "open"
        }), !t.isSupported || !t.isAllowed)
            return this.toggleAttribute("install-unsupported", !0), void (this.renderRoot.innerHTML = t.isAllowed ? "<slot name='unsupported'>Your browser does not support installing things on ESP devices. Use Google Chrome or Microsoft Edge.</slot>" : "<slot name='not-allowed'>You can only install ESP devices on HTTPS websites or on the localhost.</slot>");
        this.toggleAttribute("install-supported", !0),
        this.addEventListener("mouseover", t.preload);
        const e = document.createElement("slot");
        e.addEventListener("click", (async t => {
            t.preventDefault();
            (await import("./start-flash-9b1cffdc.js")).startFlash(this)
        })),
        e.name = "activate";
        const n = document.createElement("button");
        if (n.innerText = "INSTALL", e.append(n), "adoptedStyleSheets" in Document.prototype && "replaceSync" in CSSStyleSheet.prototype) {
            const e = new CSSStyleSheet;
            e.replaceSync(t.style),
            this.renderRoot.adoptedStyleSheets = [e]
        } else {
            const e = document.createElement("style");
            e.innerText = t.style,
            this.renderRoot.append(e)
        }
        this.renderRoot.append(e)
    }
}
t.isSupported = "serial" in navigator,
t.isAllowed = window.isSecureContext,
t.style = '\n  button {\n    position: relative;\n    cursor: pointer;\n    font-size: 14px;\n    padding: 8px 28px;\n    color: var(--esp-tools-button-text-color, #fff);\n    background-color: var(--esp-tools-button-color, #03a9f4);\n    border: none;\n    border-radius: 4px;\n    box-shadow: 0 2px 2px 0 rgba(0,0,0,.14), 0 3px 1px -2px rgba(0,0,0,.12), 0 1px 5px 0 rgba(0,0,0,.2);\n  }\n  button::before {\n    content: " ";\n    position: absolute;\n    top: 0;\n    bottom: 0;\n    left: 0;\n    right: 0;\n    opacity: 0.2;\n    border-radius: 4px;\n  }\n  button:hover {\n    box-shadow: 0 4px 8px 0 rgba(0,0,0,.14), 0 1px 7px 0 rgba(0,0,0,.12), 0 3px 1px -1px rgba(0,0,0,.2);\n  }\n  button:hover::before {\n    background-color: rgba(255,255,255,.8);\n  }\n  button:focus {\n    outline: none;\n  }\n  button:focus::before {\n    background-color: white;\n  }\n  button:active::before {\n    background-color: grey;\n  }\n  :host([active]) button {\n    color: rgba(0, 0, 0, 0.38);\n    background-color: rgba(0, 0, 0, 0.12);\n    box-shadow: none;\n    cursor: unset;\n    pointer-events: none;\n  }\n  improv-wifi-launch-button {\n    display: block;\n    margin-top: 16px;\n  }\n  .hidden {\n    display: none;\n  }',
customElements.define("esp-web-install-button", t);

