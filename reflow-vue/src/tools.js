export function getApiUrl (protocol, suffix) {
  return protocol + '://' + new URL(window.location).hostname + suffix
  // return protocol + '://reflow-oven.local' + suffix
}
