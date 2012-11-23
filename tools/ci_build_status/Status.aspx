<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Status.aspx.cs" Inherits="Status" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
  <link href="style.css" rel="stylesheet" type="text/css" />
    <title>CI Build Status</title>
    <link rel="shortcut icon" href="images/favicon.ico" />
    <link rel="icon" type="image/ico" href="images/favicon.ico" />
</head>
<body style="height: 541px">
  <form id="form1" runat="server">
    <div id="header-container">CI Build Results</div>
<table id="gradient-style">
    <thead>
      <tr>
          <th scope="col" class="auto-style1">OS &amp; Build Type</th>
            <th scope="col">Status</th>
            <th scope="col">Project</th>
            <th scope="col">Last Commiter</th>
        </tr>
    </thead>
    <tbody>
      <tr>
        <td class="auto-style1">
          <asp:Label ID="win7_64_title" runat="server" CssClass="title_column" Width="100%" Text="Windows 7 - x64"></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_64_status" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_64_project" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_64_commiter" runat="server" Text=""></asp:Label>
        </td>
      </tr>
      <tr>
        <td class="auto-style1">
          <asp:Label ID="win7_86_title" runat="server" CssClass="title_column" Width="100%" Text="Windows 7 - x86"></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_86_status" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_86_project" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="win7_86_commiter" runat="server" Text=""></asp:Label>
        </td>
      </tr>
            <tr>
        <td class="auto-style1">
          <asp:Label ID="linux_64_title" runat="server" CssClass="title_column" Width="100%" Text="Linux - x64"></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_64_status" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_64_project" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_64_commiter" runat="server" Text=""></asp:Label>
        </td>
      </tr>
            <tr>
        <td class="auto-style1">
          <asp:Label ID="linux_86_title" runat="server" CssClass="title_column" Width="100%" Text="Linux - x86"></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_86_status" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_86_project" runat="server" Text=""></asp:Label>
        </td>
          <td>
            <asp:Label ID="linux_86_commiter" runat="server" Text=""></asp:Label>
        </td>
      </tr>
    </tbody>
</table>
  </form>
</body>
</html>
