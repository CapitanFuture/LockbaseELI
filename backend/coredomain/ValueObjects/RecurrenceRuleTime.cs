using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Text.RegularExpressions; 

using Lockbase.CoreDomain.Enumerations;
using Lockbase.CoreDomain.Extensions;

namespace Lockbase.CoreDomain.ValueObjects  {
	
	public class RecurrenceRuleTime : RecurrenceRule {

		public static implicit operator RecurrenceRuleTime(string value) {

			RecurrenceRule rule = value;
			return rule as RecurrenceRuleTime;
		}

		public RecurrenceRuleTime(int multiplier, TimeInterval frequency, string values) : base(multiplier, frequency)
		{
			if (frequency == TimeInterval.Second || frequency == TimeInterval.Minute)
				this.Times = GetTimesSet(values, 0, 59);
			else if (frequency == TimeInterval.Hour)
				this.Times = GetTimesSet(values, 0, 23);
			else if (frequency == TimeInterval.WeekOfYear)
				this.Times = GetTimesSet(values, 1, 52);
			else if (frequency == TimeInterval.Month)
				this.Times = GetTimesSet(values, 1, 12);
			else if (frequency == TimeInterval.DayOfMonth)
				this.Times = GetTimesSet(values, 1, 31);
			else if (frequency == TimeInterval.DayOfYear)
				this.Times = GetTimesSet(values, 1, 366);
			else if (frequency == TimeInterval.Year)
				this.Times = GetTimesSet(values, 2000, 2099);
			else throw new ArgumentException(nameof(frequency), frequency.Name);
		}

		public IImmutableSet<int> Times { get; private set; }

		

		private static IImmutableSet<int> GetTimesSet(string values, int start, int end) {
			
			if (string.IsNullOrEmpty(values))
				return ImmutableHashSet<int>.Empty.AddRange(Enumerable.Range(start, end-start+1));
			
			var seed = ReductionState<int>.Default();

			// https://regex101.com/r/P8yGDm/2
			RegexOptions options = RegexOptions.IgnoreCase;
			string input = values.Substring(1, values.Length - 2);
			string pattern = @"([+\-])|(\d{1,4})";
			
			return Regex.Matches(input, pattern, options)
				.Cast<Match>()
				.Select(  match => match.Value )
				.Aggregate(seed, (accu,current) => 
					{
						if (current == "+" || current == "-")
						{
							return new ReductionState<int>( accu.Reduction, (current == "+") ? Operator.Add : Operator.Range );
						}
						var operand = Convert.ToInt32(current);

						if (accu.Op == Operator.Add)
							return new ReductionState<int>( accu.Reduction.Add(operand), accu.Op);
						else if (accu.Reduction.Count == 0) 
							return new ReductionState<int>( accu.Reduction.Add(-operand), Operator.Add ); 

						var lastOperand = accu.Reduction.Last();

						var range = GetRange(lastOperand, operand);
							
						return new ReductionState<int>( accu.Reduction.AddRange(range), Operator.Add );  
					})
				.Reduction;
		}

		private static IEnumerable<int> GetRange(int start, int end) {
			return Enumerable.Range(start, end-start+1);
		}
	}
}