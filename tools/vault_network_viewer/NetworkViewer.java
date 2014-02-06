package MaidSafeTools;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import javax.swing.*;


public class NetworkViewer extends JComponent {
    private static String BuildDirectory;
    private static final int WIDE = 1800;
    private static final int HIGH = 900;
    private static final int WIDTH = 220;
    private static final int HEIGHT = 120;
    private static final int LINEWIDTH = 37;
    private ControlPanel control = new ControlPanel();
    private TextPanel text_panel = new TextPanel();
    private Kind kind = Kind.Circular;
    private List<Node> nodes = new ArrayList<Node>();
    private List<Edge> edges = new ArrayList<Edge>();
    private static final NoDisplay no_display = new NoDisplay();
    private static final Display display = new Display();
    private List<Vault> vaults = new ArrayList<Vault>();
    private Point mousePt = new Point(WIDE / 2, HIGH / 2);
    private Rectangle mouseRect = new Rectangle();
    private boolean selecting = false;

    public static void main(String[] args) throws Exception {
        BuildDirectory = args[0];
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                JFrame f = new JFrame("VaultsNetworkViewer");
                f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                NetworkViewer network_viewer = new NetworkViewer();
                f.add(network_viewer.control, BorderLayout.NORTH);
                f.add(new JScrollPane(network_viewer), BorderLayout.CENTER);
                f.pack();
                f.setLocationByPlatform(true);
                f.setVisible(true);
            }
        });
    }

    public NetworkViewer() {
        this.setOpaque(true);
        this.addMouseListener(new MouseHandler());
        this.addMouseMotionListener(new MouseMotionHandler());
        for (int i = 2; i < 38; i++) {
            int x = WIDTH / 2 + 20 + ((i - 2) % 6) * 300;
            int y = HEIGHT / 2 + 10 + ((i - 2) / 6) * 150;
            Point p = new Point(x, y);
            nodes.add(new Node(p, WIDTH, HEIGHT, Color.LIGHT_GRAY, kind, i));
            String log_file = BuildDirectory + "vault_" + i + ".txt";
            vaults.add(new Vault(nodes.get(nodes.size() - 1), log_file));
        }
        no_display.LoadKeyList(BuildDirectory + "key_list.txt");
        no_display.LoadList(BuildDirectory + "no_display_list.txt");
        display.LoadList(BuildDirectory + "display_list.txt");
        new javax.swing.Timer(1000, taskPerformer).start();  //milliseconds
    }
    
    
    ActionListener taskPerformer = new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent evt) {
        repaint();
      }
    };

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(WIDE, HIGH);
    }

    @Override
    public void paintComponent(Graphics g) {
        g.setColor(new Color(0x00f0f0f0));
        g.fillRect(0, 0, getWidth(), getHeight());
        for (Edge e : edges) {
            e.draw(g);
        }
        for (Node n : nodes) {
            n.draw(g);
        }
        for (Vault vault : vaults) {
            vault.draw(g);
        }
        if (selecting) {
            g.setColor(Color.darkGray);
            g.drawRect(mouseRect.x, mouseRect.y,
                mouseRect.width, mouseRect.height);
        }
    }

    private static class NoDisplay {
        NoDisplay() {
            no_display_list_ = new ArrayList<String>();
        }
        
        void Clear() {
            no_display_list_.clear();
        }
        
        void LoadKeyList(String file_path) {
            try {
                BufferedReader txt_br = null;
                txt_br = new BufferedReader(new FileReader(file_path));
                String sCurrentLine;
                while ((sCurrentLine = txt_br.readLine()) != null) {
                    String[] splits = sCurrentLine.split(" ");
                    if (splits.length > 6) {
                        no_display_list_.add(splits[2].trim());
                        no_display_list_.add(splits[4].trim());
                        no_display_list_.add(splits[6].trim());
                    }
                }
            } catch (Exception e)
            {
                System.out.println(e.getMessage());
            }
        }

        void LoadList(String file_path) {
            try {
                BufferedReader txt_br = null;
                txt_br = new BufferedReader(new FileReader(file_path));
                String sCurrentLine;
                while ((sCurrentLine = txt_br.readLine()) != null) {
                    no_display_list_.add(sCurrentLine.trim());
                }
            } catch (Exception e)
            {
                System.out.println(e.getMessage());
            }
        }

        Boolean CanDisplay(String content) {
            for (String exception : no_display_list_) {
                if (content.contains(exception) || content.equalsIgnoreCase(exception)) {
                    return false;
                }
            }
            return true;
        }
        ArrayList<String> no_display_list_;
    }

    private static class Content {
        public Color color;
        public String content;
        public Boolean valid;
        public Persona persona;

        public Content() {
            this.valid = false;
            this.persona = Persona.Down;
        }

        public Content(Color color, String content) {
            this.color = color;
            this.content = content;
            this.valid = true;
            this.persona = Persona.None;
        }

        public Content(Color color, String content, Persona persona) {
            this.color = color;
            this.content = content;
            this.valid = true;
            this.persona = persona;
        }

        public Content(Content content) {
            this.color = content.color;
            this.content = content.content;
            this.valid = true;
            this.persona = content.persona;
        }

        public Content Clone() {
            Content clone = new Content(this.color, this.content, this.persona);
            return clone;
        }
    }
    
    private static class Display {
        ArrayList<DisplayPhrase> display_list_;

        private class DisplayPhrase {
            public Content content;
            public Boolean has_exception;

            public DisplayPhrase(String phrase) {
                String[] splits = phrase.split("><");
                if (splits[0].equalsIgnoreCase("NoException")) {
                    this.has_exception = false;
                } else {
                    this.has_exception = true;
                }
                Persona persona = Persona.valueOf(splits[1]);
                LogLevel log_level = LogLevel.valueOf(splits[2]);
                Color color = Color.BLACK;

                switch (log_level) {
                    case Error:
                        color = MaidsafeColor.Error.color;
                        break;
                    case Warning:
                        color = MaidsafeColor.Warning.color;
                        break;
                    case Info:
                        color = MaidsafeColor.Info.color;
                        break;
                    case Verbose:
                        color = MaidsafeColor.Verbose.color;
                        break;
                    case None:
                        color = Color.BLACK;
                        break;
                    default:
                        break;
                }
                this.content = new Content(color, splits[3], persona);

            }
            
            Boolean IsPhrase(String phrase) {
                if (phrase.contains(content.content)) {
                    if (has_exception) {
                        if (!no_display.CanDisplay(phrase)) {
                            return false;
                        }
                    }
                    return true;
                }
                return false;
            }
        }        

        Display() {
            display_list_ = new ArrayList<DisplayPhrase>();
        }
        
        void LoadList(String file_path) {
            try {
                BufferedReader txt_br = null;
                txt_br = new BufferedReader(new FileReader(file_path));
                String sCurrentLine;
                while ((sCurrentLine = txt_br.readLine()) != null) {
                    display_list_.add(new DisplayPhrase(sCurrentLine));
                }
            } catch (Exception e)
            {
                System.out.println(e.getMessage());
            }
        }

        Content ParsePhrase(String input) {
            Content content = new Content();
            try {
                String[] string_splits = input.split("]");
                input = string_splits[1].trim();
            } catch (Exception e) {
            }
            for (DisplayPhrase display_phrase : display_list_) {
                if (display_phrase.IsPhrase(input)) {
                    content = display_phrase.content.Clone();
                    content.content = input;
                    return content;
                }
            }
            return content;
        }
    }
    
    private static class Vault {
        private Node n;
        private String log_file_path;

        public Vault(Node n, String log_file_path) {
            this.n = n;
            this.log_file_path = log_file_path;
        }

        public void draw(Graphics g) {
            Point p = n.getTopLeft();
            try {
                BufferedInputStream txt_br = null;
                txt_br = new BufferedInputStream(new FileInputStream(log_file_path));
                int bytes = txt_br.available();
                if (bytes > 0) {
                    byte content[] = new byte[bytes];
                    txt_br.read(content);
                    String sContent = new String(content, "UTF-8");
                    String lines[] = sContent.split("\\r?\\n");
                    int j = 0;
                    for (int i = 0; i < lines.length; i++) {
                        Content parsed_content = display.ParsePhrase(lines[i]);
                        if (parsed_content.valid) {
                            updatePersona(parsed_content.persona);
                            g.setColor(parsed_content.color);
                            for (int k = 0; k <= (parsed_content.content.length() / LINEWIDTH); k++) {
                                Math.min(k, k);
                                String sub_content = parsed_content.content.substring(k * LINEWIDTH,
                                        Math.min((k + 1) * LINEWIDTH, parsed_content.content.length()));
                                if (sub_content.length() > 1) {
                                    g.drawString(sub_content, p.x - 10, p.y + j * 15);
                                    j = j + 1;
                                }
                            }
                        }
                    }
                }
                txt_br.close();
            } catch (Exception e) {
                updatePersona(Persona.Down);
                // System.out.println(e.getMessage());
            }
        }
        
        private void updatePersona(Persona persona) {
            switch (persona) {
                case MaidManager:
                    n.color = MaidsafeColor.MaidManager.color; n.kind = Kind.Circular;
                    break;
                case DataManager:
                    n.color = MaidsafeColor.DataManager.color; n.kind = Kind.Square;
                    break;
                case PmidManager:
                    n.color = MaidsafeColor.PmidManager.color; n.kind = Kind.Square;
                    break;
                case PmidNode:
                    n.color = MaidsafeColor.PmidNode.color; n.kind = Kind.Rounded;
                    break;
                case None:
                    n.color = MaidsafeColor.None.color; n.kind = Kind.Circular;
                    break;
                default:
                    n.color = Color.LIGHT_GRAY; n.kind = Kind.Circular;
                    break;
            }
        }

    }

    private class MouseHandler extends MouseAdapter {

        @Override
        public void mouseReleased(MouseEvent e) {
            selecting = false;
            mouseRect.setBounds(0, 0, 0, 0);
            e.getComponent().repaint();
        }

        @Override
        public void mousePressed(MouseEvent e) {
            mousePt = e.getPoint();
            if (e.isShiftDown()) {
                Node.selectToggle(nodes, mousePt);
            } else if (e.isPopupTrigger()) {
                Node.selectOne(nodes, mousePt);
                showPopup(e);
            } else if (Node.selectOne(nodes, mousePt)) {
                selecting = false;
            } else {
                Node.selectNone(nodes);
                selecting = true;
            }
            e.getComponent().repaint();
        }
        
        private void showPopup(MouseEvent e) {
            control.popup.show(e.getComponent(), e.getX(), e.getY());
        }

    }

    private class MouseMotionHandler extends MouseMotionAdapter {

        Point delta = new Point();

        @Override
        public void mouseDragged(MouseEvent e) {
            if (selecting) {
                mouseRect.setBounds(
                    Math.min(mousePt.x, e.getX()),
                    Math.min(mousePt.y, e.getY()),
                    Math.abs(mousePt.x - e.getX()),
                    Math.abs(mousePt.y - e.getY()));
                Node.selectRect(nodes, mouseRect);
            } else {
                delta.setLocation(
                    e.getX() - mousePt.x,
                    e.getY() - mousePt.y);
                Node.updatePosition(nodes, delta);
                mousePt = e.getPoint();
            }
            e.getComponent().repaint();
        }
    }

    public JToolBar getControlPanel() {
        return control;
    }

    private class PreConditionsAction extends AbstractAction {
        public PreConditionsAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            try {
                text_panel.Display();
                text_panel.Append("Generating keys ... ");
                
                 Runnable r = new Runnable() {
                     public void run() {
                         try {
                            Process generate_keys = Runtime.getRuntime().exec(BuildDirectory + "vault_key_helper -cp -n 40");
                            BufferedReader b = new BufferedReader(new InputStreamReader(generate_keys.getInputStream()));
                            String line = "";
                            while ((line = b.readLine()) != null) {
                              text_panel.Append(line);
                            }

                            ProcessBuilder pb = new ProcessBuilder(BuildDirectory + "vault_key_helper", "-lp");
                            pb.redirectOutput(new File(BuildDirectory + "key_list.txt"));
                            pb.start();
                            text_panel.Append("Generating chunks ... ");
                            Process generate_chunk = Runtime.getRuntime().exec(BuildDirectory + "vault_key_helper -g --chunk_set_count 5");
                            generate_chunk.waitFor();
                            text_panel.Append("Preconditions setup completed");
                            no_display.Clear();
                            Thread.sleep(3000);
                            no_display.LoadKeyList(BuildDirectory + "key_list.txt");
                            no_display.LoadList(BuildDirectory + "no_display_list.txt");
                            text_panel.Hide();
                        } catch (Exception exception) {
                            System.out.println(exception.getMessage());
                        }
                     }
                 };

                 new Thread(r).start();
            } catch (Exception exception) {
                System.out.println(exception.getMessage());
            }
            repaint();
        }
    }
    
    private class BootstrapNetworkAction extends AbstractAction {
        public BootstrapNetworkAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            try {
                text_panel.Display();
                text_panel.Append("Bootstraping ... ");
                
                 Runnable r = new Runnable() {
                     public void run() {
                         try {
                            Process p = Runtime.getRuntime().exec(BuildDirectory + "vault_key_helper -b");
                            BufferedReader b = new BufferedReader(new InputStreamReader(p.getInputStream()));
                            String line = "";
                            String endpoint_info = "";
                            while ((line = b.readLine()) != null) {
                              text_panel.Append(line);
                              if (line.contains("Endpoints")) {
                                  String[] splits = line.split(" ", -1);
                                  endpoint_info = splits[1].trim();
                                  break;
                              }
                            }
                            text_panel.Append("starting vault 2");
                            ProcessBuilder pb_vault2 = new ProcessBuilder(BuildDirectory + "vault",
                                   "--log_*", "G", "--identity_index", "2", "--peer", endpoint_info);
                            pb_vault2.redirectOutput(new File(BuildDirectory + "vault_2.txt"));
                            pb_vault2.redirectError(new File(BuildDirectory + "vault_2.txt"));
                            pb_vault2.start();
                            Thread.sleep(1000);
                            text_panel.Append("starting vault 3");
                            ProcessBuilder pb_vault3 = new ProcessBuilder(BuildDirectory + "vault",
                                   "--log_*", "G", "--identity_index", "3");
                            pb_vault3.redirectOutput(new File(BuildDirectory + "vault_3.txt"));
                            pb_vault3.redirectError(new File(BuildDirectory + "vault_3.txt"));
                            pb_vault3.start();
                            Thread.sleep(5000);
                            text_panel.Append("destroying bootstraps");
                            p.destroy();
                            Thread.sleep(5000);

                            for (int i = 4; i < 38; i++) {
                                text_panel.Append("starting vault " + i + " ...");
                                String log_file = BuildDirectory + "vault_" + i + ".txt";
                                ProcessBuilder pb = new ProcessBuilder(BuildDirectory + "vault",
                                       "--log_*", "G", "--identity_index", Integer.toString(i));
                                pb.redirectOutput(new File(log_file));
                                pb.redirectError(new File(log_file));
                                pb.start();
                                Thread.sleep(1000);
                            }
                            text_panel.Append("Network setup completed");
                            Thread.sleep(5000);
                            text_panel.Hide();
                        } catch (Exception exception) {
                            System.out.println(exception.getMessage());
                        }
                     }
                 };

                 new Thread(r).start();
            } catch (Exception exception) {
                System.out.println(exception.getMessage());
            }
            repaint();
        }
    }
    
    private class ShutDownNetworkAction extends AbstractAction {
        public ShutDownNetworkAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            try {
                text_panel.Display();
                text_panel.Append("Shutdown network, terminating vaults ... ");
                
                 Runnable r = new Runnable() {
                     public void run() {
                         try {
                            Runtime.getRuntime().exec("killall -vw -s SIGINT vault");
                            Thread.sleep(5000);
                            text_panel.Append("Cleaning up resources ... ");
                            Runtime.getRuntime().exec("rm -rf " + BuildDirectory + "????-????-????-????" );
                            for (int i = 2; i < 38; ++i) {
                                Runtime.getRuntime().exec("rm -rf /tmp/vault_chunks/vault_" + i);
                                Runtime.getRuntime().exec("mv " + BuildDirectory + "vault_"+ i + ".txt " + BuildDirectory + "logs/");
                            }
                            text_panel.Append("Network shutdown completed");
                            Thread.sleep(5000);
                            text_panel.Hide();
                        } catch (Exception exception) {
                            System.out.println(exception.getMessage());
                        }
                     }
                 };

                 new Thread(r).start();
            } catch (Exception exception) {
                System.out.println(exception.getMessage());
            }
            repaint();
        }
    }
    
    private class StartVaultAction extends AbstractAction {
        public StartVaultAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            List<Node> selected = new ArrayList<Node>();
            Node.getSelected(nodes, selected);
            String index = Integer.toString(selected.get(0).index);
            try {
                ProcessBuilder pb = new ProcessBuilder(BuildDirectory + "vault",
                       "--log_*", "G", "--identity_index", index);
                pb.redirectOutput(new File(BuildDirectory + "vault_" + index + ".txt"));
                pb.redirectError(new File(BuildDirectory + "vault_" + index + ".txt"));
                pb.start();
            } catch (Exception exception) {
                System.out.println(exception.getMessage());
            }
            repaint();
        }
    }
    
    private class StopVaultAction extends AbstractAction {
        public StopVaultAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            List<Node> selected = new ArrayList<Node>();
            Node.getSelected(nodes, selected);
            String log_file = BuildDirectory + "vault_" + selected.get(0).index + ".txt";
            String exec_cmd = "vault --log_* G --identity_index " + selected.get(0).index;
            try {
                Process p = Runtime.getRuntime().exec("ps -aux");
                p.waitFor();
                BufferedReader b = new BufferedReader(new InputStreamReader(p.getInputStream()));
                String line = "";
                String thread = "";
                while (((line = b.readLine()) != null) && (thread.equals(""))) {
                  if (line.contains(exec_cmd)) {
                      String[] splits = line.split(" ", -1);
                      thread = splits[1].trim();
                  }
                }
                Runtime.getRuntime().exec("kill -9 " + thread);
                Runtime.getRuntime().exec("rm " + log_file);
            } catch (Exception exception) {
                System.out.println(exception.getMessage());
            }
            repaint();
        }
    }
    
    private class TextPanel extends JFrame {
        private JTextArea output;
        private JScrollPane scrollPane;
        private String newline = "\n";
    
        TextPanel() {
            super("Output");
            Container popup_container = createContentPane();
            popup_container.setEnabled(true);
            this.add(popup_container, BorderLayout.CENTER);
            this.setLocationByPlatform(true);
            this.pack();
            this.setAutoRequestFocus(true);
            new javax.swing.Timer(500, rePainter).start();  //milliseconds
        }
        public void Append(String txt) {
            output.append(txt + newline);
            output.setCaretPosition(output.getDocument().getLength());
        }
        public void Display() {
            this.setVisible(true);
            this.toFront();
        }
        public void Hide() {
            output.setText("");
            this.setVisible(false);
            this.toBack();
        }
        private Container createContentPane() {
            JPanel contentPane = new JPanel(new BorderLayout());
            contentPane.setOpaque(true);
            output = new JTextArea(10, 50);
            output.setEditable(false);
            scrollPane = new JScrollPane(output);
            contentPane.add(scrollPane, BorderLayout.CENTER);
            return contentPane;
        }
        ActionListener rePainter = new ActionListener() {
          @Override
          public void actionPerformed(ActionEvent evt) {
            repaint();
          }
        };
    }
    
    private class ControlPanel extends JToolBar {
        private Action preconditions = new PreConditionsAction("PreConditions");
        private Action bootstrap_network = new BootstrapNetworkAction("Bootstrap Network");
        private Action shut_down_network = new ShutDownNetworkAction("Shutdown Network");
        private Action start = new StartVaultAction("Start");
        private Action stop = new StopVaultAction("Stop");
        private JPopupMenu popup = new JPopupMenu();

        ControlPanel() {
            this.setLayout(new FlowLayout(FlowLayout.LEFT));
            this.setBackground(Color.lightGray);
            this.add(new JButton(preconditions));
            this.add(new JButton(bootstrap_network));
            this.add(new JButton(shut_down_network));
            popup.add(new JMenuItem(start));
            popup.add(new JMenuItem(stop));
        }        
    }
    
    private enum Kind {
        Circular, Rounded, Square;
    }
    
    private enum Persona {
        None, MaidManager, DataManager, PmidManager, PmidNode, Down;
    }

    private enum LogLevel {
        None, Verbose, Info, Warning, Error;
    }

    public enum MaidsafeColor {
        MaidManager (new Color(249, 185, 194)), DataManager (new Color(249, 246, 138)),
        PmidManager (new Color(213, 242, 160)), PmidNode (new Color(199, 219, 234)), None (new Color(213, 242, 160)),
        Verbose (new Color(102, 102, 102)), Info (new Color(14, 76, 114)),
        Warning (new Color(188, 23, 102)), Error (new Color(193, 39, 45));
        private Color color;
        private MaidsafeColor(Color color) {
            this.color = color;
        }
    }

    private static class Edge {

        private Node n1;
        private Node n2;

        public Edge(Node n1, Node n2) {
            this.n1 = n1;
            this.n2 = n2;
        }

        public void draw(Graphics g) {
            Point p1 = n1.getLocation();
            Point p2 = n2.getLocation();
            g.setColor(Color.darkGray);
            g.drawLine(p1.x, p1.y, p2.x, p2.y);
        }
    }

    private static class Node {
        public Color color;
        public Kind kind;
        public int index;

        private Point p;
        private int width;
        private int height;
        private boolean selected = false;
        private Rectangle b = new Rectangle();

        /**
         * Construct a new node.
         */
        public Node(Point p, int width, int height,
                    Color color, Kind kind, int index) {
            this.p = p;
            this.width = width;
            this.height = height;
            this.color = color;
            this.kind = kind;
            this.index = index;
            setBoundary(b);
        }

        /**
         * Calculate this node's rectangular boundary.
         */
        private void setBoundary(Rectangle b) {
            b.setBounds(p.x - (width / 2), p.y - (height / 2), width, height);
        }

        /**
         * Draw this node.
         */
        public void draw(Graphics g) {
            g.setColor(this.color);
            if (this.kind == Kind.Circular) {
                g.fillOval(b.x, b.y, b.width, b.height);
            } else if (this.kind == Kind.Rounded) {
                g.fillRoundRect(b.x, b.y, b.width, b.height, 150, 90);
                
            } else if (this.kind == Kind.Square) {
                g.fillRect(b.x, b.y, b.width, b.height);
            }
            if (selected) {
                g.setColor(Color.darkGray);
                g.drawRect(b.x, b.y, b.width, b.height);
            }
        }

        /**
         * Return this node's location.
         */
        public Point getLocation() {
            return p;
        }
        
        public Point getTopLeft() {
            Point tl = new Point();
            tl.x = p.x - width / 2 + 10;
            tl.y = p.y - height / 2 + 10;
            return tl;
        }

        /**
         * Return true if this node contains p.
         */
        public boolean contains(Point p) {
            return b.contains(p);
        }

        /**
         * Return true if this node is selected.
         */
        public boolean isSelected() {
            return selected;
        }

        /**
         * Mark this node as selected.
         */
        public void setSelected(boolean selected) {
            this.selected = selected;
        }

        /**
         * Collected all the selected nodes in list.
         */
        public static void getSelected(List<Node> list, List<Node> selected) {
            selected.clear();
            for (Node n : list) {
                if (n.isSelected()) {
                    selected.add(n);
                }
            }
        }

        /**
         * Select no nodes.
         */
        public static void selectNone(List<Node> list) {
            for (Node n : list) {
                n.setSelected(false);
            }
        }

        /**
         * Select a single node; return true if not already selected.
         */
        public static boolean selectOne(List<Node> list, Point p) {
            for (Node n : list) {
                if (n.contains(p)) {
                    if (!n.isSelected()) {
                        Node.selectNone(list);
                        n.setSelected(true);
                    }
                    return true;
                }
            }
            return false;
        }

        /**
         * Select each node in r.
         */
        public static void selectRect(List<Node> list, Rectangle r) {
            for (Node n : list) {
                n.setSelected(r.contains(n.p));
            }
        }

        /**
         * Toggle selected state of each node containing p.
         */
        public static void selectToggle(List<Node> list, Point p) {
            for (Node n : list) {
                if (n.contains(p)) {
                    n.setSelected(!n.isSelected());
                }
            }
        }

        /**
         * Update each node's position by d (delta).
         */
        public static void updatePosition(List<Node> list, Point d) {
            for (Node n : list) {
                if (n.isSelected()) {
                    n.p.x += d.x;
                    n.p.y += d.y;
                    n.setBoundary(n.b);
                }
            }
        }

    }
    
}
