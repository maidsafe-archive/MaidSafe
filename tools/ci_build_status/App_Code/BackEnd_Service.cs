using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Web;
using System.Web.Configuration;
using System.Web.Services;

[System.Web.Script.Services.ScriptService]
public class BackEndService : System.Web.Services.WebService {
  public BackEndService () {
  }

  [WebMethod]
  public void SubmitBuildResult(string input_string)
  {
    try {
      NameValueCollection query_string = HttpUtility.ParseQueryString(input_string);
      string os = query_string["os"];
      string build_type = query_string["build_type"];
      string status = query_string["status"];

      if (string.IsNullOrEmpty(os) || string.IsNullOrEmpty(build_type) ||
          string.IsNullOrEmpty(status))
        return;

      if (status != "ok" && status != "fail")
        return;

#region Build OK
      if (status == "ok") {
        AddToConfigFile(os + "," + build_type, status);
        return;
      }
#endregion

#region Build Failure
      if (status == "fail") {
        string project = query_string["project"];
        string commiter = query_string["commiter"];

        if (string.IsNullOrEmpty(project) || string.IsNullOrEmpty(commiter))
          return;

        AddToConfigFile(os + "," + build_type, project + "," + commiter);
        return;
      }
#endregion

    } catch(Exception /*ex*/) {
      return;
    }
  }

  public void AddToConfigFile(string key, string value) {
    Configuration config = WebConfigurationManager.OpenWebConfiguration(null);
    config.AppSettings.Settings.Remove(key);
    config.Save(ConfigurationSaveMode.Modified);
    ConfigurationManager.RefreshSection("appSettings");
    config.AppSettings.Settings.Add(key, value);
    config.Save(ConfigurationSaveMode.Modified);
    ConfigurationManager.RefreshSection("appSettings");
  }
}