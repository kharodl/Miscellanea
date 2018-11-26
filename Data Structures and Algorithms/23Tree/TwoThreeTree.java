import java.util.ArrayList;
import java.util.Collections;

public class TwoThreeTree {
	private Node root;

	public TwoThreeTree() {
		root = null;
	}

	public boolean insert(int element) {
		if (root == null)
			root = new Node();
		Node currentNode = root.findNode(element);
		if (currentNode.elements.contains(element))
			return false;
		currentNode.insertElement(element);
		return true;
	}

	public String search(int element) {
		Node currentNode = root.findNode(element);
		switch (currentNode.elements.size()) {
			case 1:
				return currentNode.elements.get(0) + "";
			default:
				return currentNode.elements.get(0) + " " + currentNode.elements.get(1);
		}
	}

	private class Node {
		private Node parent;
		private ArrayList<Integer> elements;
		private ArrayList<Node> children;

		private Node() {
			elements = new ArrayList<Integer>();
			children = new ArrayList<Node>();
			parent = null;
		}

		public void insertElement(int element) {
			elements.add(element);
			Collections.sort(elements);
			if (elements.size() == 3)
				splitNode();
		}

		public Node findNode(int element) { // Finds a node in the appropriate location for or of an element.
			if (children.isEmpty() || elements.contains(element))
				return this;
			int index = 0;
			for (int i = 0; i < elements.size(); i++)
				if (element > elements.get(i))
					index = i + 1;
			return children.get(index).findNode(element);
		}

		public void splitNode() {
			if (this == root) {
				root = parent = new Node();
				parent.children.add(this);
			}
			Node nodeSibling = new Node();
			nodeSibling.elements.add(this.elements.remove(2));
			nodeSibling.parent = this.parent;
			parent.children.add(parent.children.indexOf(this) + 1, nodeSibling);
			while (this.children.size() > 2) {
				nodeSibling.children.add(this.children.remove(2));
				nodeSibling.children.get(nodeSibling.children.size() - 1).parent = nodeSibling;
			}
			parent.insertElement(this.elements.remove(1));
		}
	}
}