import java.util.ArrayList;
import java.util.Collection;


public class MaxHeap {
	private ArrayList<Student> students;

	public MaxHeap(int capacity) {
		students = new ArrayList<Student>(capacity);
	}

	public MaxHeap(Collection<Student> collection) {
		students = new ArrayList<Student>(collection);
		for (int i = 0; i < size(); i++)
			students.get(i).setIndex(i);
		for (int i = size() / 2; i >= 0; i--)
			maxHeapify(i);
	}

	public Student getMax() {
		if (size() < 1)
			throw new IndexOutOfBoundsException("No maximum value:  the heap is empty.");
		return students.get(0);
	}

	public Student extractMax() {
		Student value = getMax();
		swap(0, size() - 1);
		students.remove(size() - 1);
		maxHeapify(0);
		return value;
	}

	public void insert(Student elt) {
		students.add(elt);
		elt.setIndex(size() - 1);
		bubble(size() - 1);
	}

	public void changeKey(Student s, double newGPA) {
		double oldGPA = s.gpa();
		s.setGPA(newGPA);
		if (oldGPA > s.gpa())
			maxHeapify(s.getIndex());
		else if (oldGPA < s.gpa())
			bubble(s.getIndex());
	}

	private void bubble(int index) {
		for (int i = index; students.get(parent(i)).compareTo(students.get(i)) < 0; i = parent(i))
			swap(i, parent(i));
	}

	private int parent(int index) {
		return (index - 1) / 2;
	}

	private int left(int index) {
		return 2 * index + 1;
	}

	private int right(int index) {
		return 2 * index + 2;
	}

	private int size() {
		return students.size();
	}

	private void swap(int from, int to) {
		Student val = students.get(from);
		students.set(from, students.get(to));
		students.set(to, val);
		students.get(from).setIndex(from);
		val.setIndex(to);
	}

	private void maxHeapify(int index) {
		int left = left(index);
		int right = right(index);
		int largest = index;
		if (left < size() && students.get(left).compareTo(students.get(largest)) > 0) {
			largest = left;
		}
		if (right < size() && students.get(right).compareTo(students.get(largest)) > 0) {
			largest = right;
		}
		if (largest != index) {
			swap(index, largest);
			maxHeapify(largest);
		}
	}
}
