import java.util.ArrayList;
import java.util.LinkedList;

public class JobSchedule {
	private ArrayList<Job> jobList;
	private ArrayList<Job> topOrder;
	private boolean sorted, relaxed, cyclic;
	private int MCT;

	public JobSchedule() {
		jobList = new ArrayList<Job>();
	}

	public Job addJob(int time) {
		sorted = false;
		jobList.add(new Job(time));
		return jobList.get(jobList.size() - 1);
	}

	public Job getJob(int index) {
		return jobList.get(index);
	}

	public int minCompletionTime() {
		topSort();
		if (cyclic)
			return -1;
		relax();
		return MCT;
	}

	private void topSort() {
		if (sorted)
			return;
		LinkedList<Job> zeroIn = new LinkedList<Job>();
		topOrder = new ArrayList<Job>();
		for (Job j : jobList) {
			j.tempIn = j.inDegree;
			j.startTime = 0;
			if (j.tempIn == 0)
				zeroIn.add(j);
		}
		while (!zeroIn.isEmpty()) {
			Job j = zeroIn.poll();
			topOrder.add(j);
			for (Job current : j.outgoing) {
				current.tempIn--;
				if (current.tempIn == 0)
					zeroIn.add(current);
			}
		}
		sorted = true;
		if (topOrder.size() != jobList.size())
			cyclic = true;
	}

	private void relax() {
		if (relaxed)
			return;
		for (Job j : topOrder) {
			for (Job current : j.outgoing)
				if (current.startTime < j.startTime + j.completionTime)
					current.startTime = j.startTime + j.completionTime;
			if (MCT < j.startTime + j.completionTime)
				MCT = j.startTime + j.completionTime;
		}
		relaxed = true;
	}

	class Job {
		private ArrayList<Job> outgoing;
		private int completionTime;
		private int startTime;
		private int inDegree;
		private int tempIn;

		private Job(int time) {
			completionTime = time;
			outgoing = new ArrayList<Job>();
		}

		public void requires(Job j) {
			sorted = relaxed = false;
			j.outgoing.add(this);
			inDegree++;
		}

		public int getStartTime() {
			topSort();
			if (tempIn != 0)
				return -1;
			relax();
			return startTime;
		}
	}
}