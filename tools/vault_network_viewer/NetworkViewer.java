package MaidSafeTools;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import javax.swing.*;


public class NetworkViewer extends JComponent {
    private static final String BuildDirectory = "/home/maidsafe/MaidSafe-Open/build/";
    private static final int WIDE = 1800;
    private static final int HIGH = 900;
    private static final int WIDTH = 220;
    private static final int HEIGHT = 120;
    private static final int LINEWIDTH = 37;
    private static final Random rnd = new Random();
    private ControlPanel control = new ControlPanel();
    private Kind kind = Kind.Circular;
    private List<Node> nodes = new ArrayList<Node>();
    private List<Edge> edges = new ArrayList<Edge>();
    private static final NoDisplay no_display = new NoDisplay();
    private List<LogReader> readers = new ArrayList<LogReader>();
    private Point mousePt = new Point(WIDE / 2, HIGH / 2);
    private Rectangle mouseRect = new Rectangle();
    private boolean selecting = false;

    public static void main(String[] args) throws Exception {
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
            readers.add(new LogReader(nodes.get(nodes.size() - 1), log_file));
        }
        no_display.LoadKeyList(BuildDirectory + "key_list.txt");
        no_display.LoadList(BuildDirectory + "no_display_list.txt");
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
        for (LogReader reader : readers) {
            reader.draw(g);
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

    private static class LogReader {
        private Node n;
        private String log_file_path;

        private class Content {
            public Color color;
            public String content;
            public Boolean valid;
            public Persona persona;
            
            public Content() {
                this.valid = false;
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
        }

        public LogReader(Node n, String log_file_path) {
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
                        Content parsed_content;
                        parsed_content = parseContent(lines[i]);
                        if (parsed_content.valid) {
                            n.updatePersona(parsed_content.persona);
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
                n.updatePersona(Persona.Down);
                System.out.println(e.getMessage());
            }
        }
        
        private Content parseContent(String input) {
            Content content = new Content();
            if (input.contains("Vault running as")) {
                content = new Content(Color.BLACK, input);
            } else {
                try {
                    String[] string_splits = input.split("]");
                    input = string_splits[1].trim();
                } catch (Exception e) {
                    return content;
                }
                if (no_display.CanDisplay(input)) {
                    if (input.contains("PmidNode")) {
                        if (input.contains("storing")) {  // storing chunk
                            String[] splits = input.split("with");
                            content = new Content(MaidsafeColor.Info.color, splits[0], Persona.PmidNode);
                        } else {  // deleting chunk
                            content = new Content(MaidsafeColor.Warning.color, input, Persona.PmidNode);
                        }
                    } else if (input.contains("DataManager")) {  // increase/decrease subscribers
                        content = new Content(MaidsafeColor.Verbose.color, input, Persona.DataManager);
                    } else if (input.contains("MaidManager")) {
                        if (input.contains("blocked DeleteRequest of chunk")) {
                        content = new Content(MaidsafeColor.Error.color, input, Persona.MaidManager);
                        } else {  // increase/decrease count
                            content = new Content(MaidsafeColor.Verbose.color, input, Persona.MaidManager);
                        }
                    }
                }
            }
            return content;
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

    private class StartVaultAction extends AbstractAction {
        public StartVaultAction(String name) {
            super(name);
        }
        public void actionPerformed(ActionEvent e) {
            List<Node> selected = new ArrayList<Node>();
            Node.getSelected(nodes, selected);
            String index = Integer.toString(selected.get(0).index);
            String exec_cmd = BuildDirectory + "vault --log_* G --identity_index " + index;
            System.out.println(exec_cmd);
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
                      String[] splits = line.split("  ");
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

    private class ControlPanel extends JToolBar {
        private Action start = new StartVaultAction("Start");
        private Action stop = new StopVaultAction("Stop");
        private JPopupMenu popup = new JPopupMenu();

        ControlPanel() {
            this.setLayout(new FlowLayout(FlowLayout.LEFT));
            this.setBackground(Color.lightGray);
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

    public enum MaidsafeColor {
        MaidManager (new Color(249, 185, 194)), DataManager (new Color(249, 246, 138)),
        PmidManager (Color.BLACK), PmidNode (new Color(199, 219, 234)), None (new Color(213, 242, 160)),
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

        public void updatePersona(Persona persona) {
            switch (persona) {
                case MaidManager:
                    color = MaidsafeColor.MaidManager.color; kind = Kind.Circular;
                    break;
                case DataManager:
                    color = MaidsafeColor.DataManager.color; kind = Kind.Square;
                    break;
                case PmidNode:
                    color = MaidsafeColor.PmidNode.color; kind = Kind.Rounded;
                    break;
                case None:
                    color = MaidsafeColor.None.color; kind = Kind.Circular;
                    break;
                default:
                    color = Color.LIGHT_GRAY; kind = Kind.Circular;
                    break;
            }
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
