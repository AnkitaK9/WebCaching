from metaflow import FlowSpec, step
import os
import numpy as np
import re
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score
import joblib


class TrainTraceClassifier(FlowSpec):

    @step
    def start(self):
        self.next(self.configure)

    @step
    def configure(self):
        """
        This step defines the directory paths and hyperparameters.
        """
        self.destination_dir = "trace_files_old"
        self.text_file_path = "outputwikimedia_old.txt"
        self.test_size = 0.2
        self.n_estimators = 100
        self.random_state = 42
        self.next(self.list_and_sort_files)

    @step
    def list_and_sort_files(self):
        """
        Lists the files in the destination directory and sorts them based on numbers in filenames.
        """
        self.unzipped_files = os.listdir(self.destination_dir)
        self.get_number_from_filename = lambda filename: int(filename.split('_')[1].split('.')[0])
        self.sorted_trace_files = sorted(self.unzipped_files, key=self.get_number_from_filename)
        self.next(self.process_traces)

    @step
    def process_traces(self):
        """
        Reads and processes trace files, handling potential missing data.
        """
        def process_trace_file(file_path):
            arrays = []
            with open(file_path, 'r') as file:
                for line in file:
                    parts = line.strip().split(' ')
                    if len(parts) >= 3:
                        arrays.append([int(parts[0]), int(parts[2])])
                    else:
                        print(f"Ignoring incomplete line: {line}")
            return np.array(arrays)

        self.all_arrays = [process_trace_file(os.path.join(self.destination_dir, filename))
                           for filename in self.sorted_trace_files]
        self.next(self.extract_labels)

    @step
    def extract_labels(self):
        """
        Reads the text file and extracts filenames and labels using regular expressions.
        """
        pattern = r"File: (subset_\d+\.tr)\nLeast execution time: ([\w\s]+) - \d+\.\d+ seconds."
        with open(self.text_file_path, 'r') as file:
            data = file.read()
        self.file_label_tuples = re.findall(pattern, data)
        self.get_number_from_filename = lambda filename: int(re.search(r'\d+', filename).group())
        self.sorted_tuples = sorted(self.file_label_tuples, key=lambda x: self.get_number_from_filename(x[0]))
        self.labels = [label for _, label in self.sorted_tuples]
        self.next(self.prepare_data)

    @step
    def prepare_data(self):
        """
        Prepares the training and testing data by flattening matrices and handling potential mismatches.
        """
        self.X = [arr.flatten() for arr in self.all_arrays[:-1]]  # Exclude the last element (test set)
        self.y = self.labels[:-1]
        self.next(self.train_test_split)

    @step
    def train_test_split(self):
        """
        Splits the data into training and testing sets using scikit-learn.
        """
        self.X_train, self.X_test, self.y_train, self.y_test = train_test_split(
            self.X, self.y, test_size=self.test_size, random_state=self.random_state)
        self.next(self.train_model)

    @step
    def train_model(self):
        """
        Trains a Random Forest Classifier model on the training data.
        """
        self.rf_classifier = RandomForestClassifier(n_estimators=self.n_estimators, random_state=self.random_state)
        self.rf_classifier.fit(self.X_train, self.y_train)
        self.next(self.evaluate_model)

    @step
    def evaluate_model(self):
        """
        Evaluates the model's accuracy on the testing data.
        """
        self.y_pred = self.rf_classifier.predict(self.X_test)
        self.accuracy = accuracy_score(self.y_test, self.y_pred)
        print(f"Accuracy: {self.accuracy}")
        self.next(self.save_model)

    
    @step
    def save_model(self):
        """
        Saves the trained model for later use.
        """
        joblib.dump(self.rf_classifier, "trace_classifier.pkl")
        self.next(self.end)
    
    @step
    def end(self):
        pass

if __name__ == '__main__':
    train_trace_classifier = TrainTraceClassifier()
    # train_trace_classifier.run()
    # train_trace_classifier.show()
