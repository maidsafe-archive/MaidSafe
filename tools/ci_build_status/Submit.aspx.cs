using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Configuration;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Web.Configuration;
using System.Web.Services;

public partial class Submit : System.Web.UI.Page
{
  protected void Page_Load(object sender, EventArgs e)
  {
    try
    {
      string os = Request.QueryString["os"];
      string build_type = Request.QueryString["build_type"];
      string status = Request.QueryString["status"];
      if (string.IsNullOrEmpty(os) ||
          string.IsNullOrEmpty(build_type) ||
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
        string project = Request.QueryString["project"];
        string commiter = Request.QueryString["commiter"];

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

  public void AddToConfigFile(string key, string value)
  {
    Configuration config = WebConfigurationManager.OpenWebConfiguration(null);
    config.AppSettings.Settings.Remove(key);
    config.Save(ConfigurationSaveMode.Modified);
    ConfigurationManager.RefreshSection("appSettings");
    config.AppSettings.Settings.Add(key, value);
    config.Save(ConfigurationSaveMode.Modified);
    ConfigurationManager.RefreshSection("appSettings");
  }
}